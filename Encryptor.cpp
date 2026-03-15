#include <Encryptor.h>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QCryptographicHash>
#define OPENSSL_USE_STATIC_LIBS 1
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/hmac.h>

Encryptor& Encryptor::getInstance() {
    //Объект создаём один раз при первом вызове функции, затем передаём его по ссылке
    static Encryptor instance;
    return instance;
}

bool Encryptor::traverseDirectory(const QString& path,
                                  const QString& password, bool encrypt) {
    //Функция решает подзадачу рекурсивного обхода папки по пути для шифрования/дешифрования содержимого
    QDir dir(path);
    if (!dir.exists())
    {
        QFile file(path);
        if (!file.exists())
        {
         qWarning() << "Directory or file does not exist:" << path;
        return false;
        }//Шифруем только файл если путь ведёт к файлу, а не папке
        return processFile(path, password, encrypt);
    }
    QString projectPath = QDir::current().path(); // Получили путь к папке где собирается проект, обычно собирается в папке проекта
    if (projectPath.startsWith(dir.path())) {
        qWarning() << "You cannot encrypt the folder where the project is located!";
        return false;
    }// Нельзя шифровать папку проекта
    QDirIterator itDirs(path,
                        QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                        QDirIterator::Subdirectories);
    bool success = true;
    while (itDirs.hasNext())
    {
        QFileInfo info(itDirs.next());
        if (!info.isDir()) { // Шифруем только файлы, в папки попадаем за счёт флага QDirIterator::Subdirectories
            // Вызов processFile у текущего экземпляра Singleton
            if (!processFile(info.absoluteFilePath(), password, encrypt))
                success = false;
        }
    }
    return success;
}
bool Encryptor::encryptData(const QString& path, const QString& password) {
    return traverseDirectory(path, password, true);
}

bool Encryptor::decryptData(const QString& path, const QString& password) {
    return traverseDirectory(path, password, false);
}
bool Encryptor::processFile(const QString& filePath,const QString& password, bool encrypt) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }
    file.close();
    QByteArray key = deriveKey(password);
    if (encrypt)
    {
        encryptFile(filePath, key);
        return true;
    }
    return decryptFile(filePath, key);

}

QByteArray Encryptor::deriveKey(const QString& password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    //Из пароля генерируем SHA-256 хеш, наш ключ
}

void Encryptor::encryptFile(const QString& filePath,
                            const QByteArray& key) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        qInfo() << "Error opening files for encryption.";
        return;
    }

    QByteArray data = file.readAll();

    // Проверка, не зашифрован ли уже файл
    file.seek(0);
    QByteArray header = file.read(8);

    if (header.size() == 8 && memcmp(header.constData(), "\x00ENCRYPT", 8) == 0) {
        qInfo() << "File is already encrypted. Skipping.";
        file.close();
        return;
    }

    file.seek(0);

    // Инициализация контекста шифрования
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Генерация случайного IV
    unsigned char iv[EVP_MAX_IV_LENGTH];
    RAND_bytes(iv, EVP_MAX_IV_LENGTH);

    // Генерация случайной соли для HMAC
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    // Запись заголовка файла: магическая подпись (8 байт) + соль (16 байт) + IV (16 байт)
    file.write("\x00ENCRYPT", 8);
    file.write((char*)salt, 16);
    file.write((char*)iv, EVP_MAX_IV_LENGTH);

    // Инициализация шифрования AES-256 CBC
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.data(), iv);

    // Буфер для зашифрованных данных
    std::vector<unsigned char> outBuffer(data.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen = 0;

    // Шифрование данных
    EVP_EncryptUpdate(ctx, outBuffer.data(), &outLen,
                      (const unsigned char*)data.data(), data.size());

    int finalLen = 0;
    EVP_EncryptFinal_ex(ctx, outBuffer.data() + outLen, &finalLen);
    outLen += finalLen;

    // Запись зашифрованных данных
    file.write(reinterpret_cast<char*>(outBuffer.data()), outLen);

    // Сохраняем текущую позицию ***
    qint64 endOfDataPos = file.pos();

    // Возвращаемся в начало и читаем все записанные данные ***
    file.seek(0);
    QByteArray fileDataForHMAC = file.read(endOfDataPos); // Читаем от начала до конца данных

    // Вычисление HMAC ключа из основного ключа и соли
    QByteArray hmacKey = QCryptographicHash::hash(key + QByteArray((char*)salt, 16),
                                                   QCryptographicHash::Sha256);

    // Создаем HMAC используя OpenSSL
    unsigned char hmac[32];
    unsigned int hmacLen;
    HMAC(EVP_sha256(), hmacKey.constData(), hmacKey.size(),
         (const unsigned char*)fileDataForHMAC.constData(), fileDataForHMAC.size(),
         hmac, &hmacLen);

    // Возвращаемся в конец данных и записываем HMAC ***
    file.seek(endOfDataPos);
    file.write((char*)hmac, 32);

    // Усекаем файл до фактического размера
    file.resize(file.pos());
    file.close();

    EVP_CIPHER_CTX_free(ctx);
    qInfo() << "Encrypted:" << filePath;
}

bool Encryptor::decryptFile(const QString& filePath,
                            const QByteArray& key) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {  // Используем ReadOnly для дешифровки
        qInfo() << "Error opening file for decryption.";
        return false;
    }

    // Читаем весь файл
    QByteArray fileContent = file.readAll();
    file.close();

    if (fileContent.size() < 8 + 16 + EVP_MAX_IV_LENGTH + 32) {
        qInfo() << "File is too small to contain valid encrypted data.";
        return false;
    }

    int pos = 0;

    // Проверяем сигнатуру
    QByteArray signature = fileContent.mid(pos, 8);
    pos += 8;
    if (signature.size() != 8 || memcmp(signature.constData(), "\x00ENCRYPT", 8) != 0) {
        qInfo() << "File is not encrypted or format is invalid.";
        return false;
    }

    // Читаем соль
    QByteArray salt = fileContent.mid(pos, 16);
    pos += 16;

    // Читаем IV
    QByteArray iv = fileContent.mid(pos, EVP_MAX_IV_LENGTH);
    pos += EVP_MAX_IV_LENGTH;

    // Читаем зашифрованные данные (все до последних 32 байт)
    QByteArray encryptedData = fileContent.mid(pos, fileContent.size() - pos - 32);

    // Читаем HMAC (последние 32 байта)
    QByteArray fileHMAC = fileContent.right(32);

    // Вычисляем HMAC ключ из основного ключа и соли
    QByteArray hmacKey = QCryptographicHash::hash(key + salt, QCryptographicHash::Sha256);

    // Формируем данные для проверки HMAC (все кроме последних 32 байт)
    QByteArray dataForHMAC = fileContent.left(fileContent.size() - 32);

    // Проверяем HMAC
    unsigned char computedHMAC[32];
    unsigned int hmacLen;
    HMAC(EVP_sha256(), hmacKey.constData(), hmacKey.size(),
         (const unsigned char*)dataForHMAC.constData(), dataForHMAC.size(),
         computedHMAC, &hmacLen);

    // Сравниваем HMAC
    if (hmacLen != 32 || CRYPTO_memcmp(computedHMAC, fileHMAC.constData(), 32) != 0) {
        qInfo() << "Incorrect decryption key or file corrupted.";
        return false;
    }

    // Инициализация контекста дешифрования
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                      (const unsigned char*)key.data(), (const unsigned char*)iv.data());

    // Буфер для расшифрованных данных
    std::vector<unsigned char> outBuffer(encryptedData.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen = 0;

    // Дешифрование данных
    if (EVP_DecryptUpdate(ctx, outBuffer.data(), &outLen,
                          (const unsigned char*)encryptedData.constData(),
                          encryptedData.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int finalLen = 0;
    if (EVP_DecryptFinal_ex(ctx, outBuffer.data() + outLen, &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    outLen += finalLen;

    // Запись расшифрованных данных обратно в файл
    QFile outFile(filePath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    outFile.write(reinterpret_cast<char*>(outBuffer.data()), outLen);
    outFile.close();

    EVP_CIPHER_CTX_free(ctx);
    qInfo() << "Decrypted:" << filePath;
    return true;
}
