#include <Encryptor.h>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#define OPENSSL_USE_STATIC_LIBS 1
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>


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
        qWarning() << "Directory does not exist:" << path;
        return false;
    }
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
bool Encryptor::encryptDirectory(const QString& path, const QString& password) {
    return traverseDirectory(path, password, true);
}

bool Encryptor::decryptDirectory(const QString& path, const QString& password) {
    return traverseDirectory(path, password, false);
}
bool Encryptor::processFile(const QString& filePath,
                            const QString& password, bool encrypt) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }
    file.close();

    QByteArray key = deriveKey(password);
    QString outputPath = filePath + (encrypt ? ".enc" : "");// формируем путь к файлу

    if (encrypt) {
        // Проверяем, не зашифрован ли уже файл
        if (filePath.endsWith(".enc", Qt::CaseInsensitive)) {
            qDebug() << "The file is already encrypted, skip it:" << filePath;
            return true; // Просто пропускаем
        }
        encryptFile(filePath, outputPath, key);
        QFile::remove(filePath); // Удаляем оригинал после шифрования
    } else {
        // Для дешифрования убираем расширение .enc
        if (filePath.endsWith(".enc")) {
            outputPath = filePath.left(filePath.length() - 4); // Убираем .enc
            decryptFile(filePath, outputPath, key);
            QFile::remove(filePath); // Удаляем зашифрованный файл
        } else {
            qDebug() << "Skipping non-encrypted file:" << filePath;
            return false;
        }
    }
    return true;
}

QByteArray Encryptor::deriveKey(const QString& password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    //Из пароля генерируем SHA-256 хеш, наш ключ
}

void Encryptor::encryptFile(const QString& inputPath,
                            const QString& outputPath,
                            const QByteArray& key) {
    QFile input(inputPath);
    QFile output(outputPath);

    if (!input.open(QIODevice::ReadOnly) || !output.open(QIODevice::WriteOnly)) {
        qDebug() << "Error opening files for encryption.";
        return;
    }

    QByteArray data = input.readAll();
    input.close();

    // Инициализация контекста шифрования
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Генерация случайного IV
    unsigned char iv[EVP_MAX_IV_LENGTH];
    RAND_bytes(iv, EVP_MAX_IV_LENGTH);

    // Инициализация шифрования AES-256 CBC
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.data(), iv);

    // Запись IV в начало выходного файла
    output.write((char*)iv, EVP_MAX_IV_LENGTH);

    // ИСПРАВЛЕНО: используем вектор вместо массива переменной длины
    std::vector<unsigned char> outBuffer(data.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen = 0;

    // Шифрование данных
    EVP_EncryptUpdate(ctx, outBuffer.data(), &outLen,
                      (const unsigned char*)data.data(), data.size());

    int finalLen = 0;
    EVP_EncryptFinal_ex(ctx, outBuffer.data() + outLen, &finalLen);
    outLen += finalLen;

    // Запись зашифрованных данных
    output.write(reinterpret_cast<char*>(outBuffer.data()), outLen);
    output.close();

    EVP_CIPHER_CTX_free(ctx);
    QFile::remove(inputPath);
    qDebug() << "Encrypted:" << inputPath << "->" << outputPath;
}

void Encryptor::decryptFile(const QString& inputPath,
                            const QString& outputPath,
                            const QByteArray& key) {
    QFile input(inputPath);
    QFile output(outputPath);

    if (!input.open(QIODevice::ReadOnly) || !output.open(QIODevice::WriteOnly)) {
        qDebug() << "Error opening files for decryption.";
        return;
    }

    // Чтение IV (первые EVP_MAX_IV_LENGTH байт файла)
    unsigned char iv[EVP_MAX_IV_LENGTH];
    input.read((char*)iv, EVP_MAX_IV_LENGTH);

    // Чтение зашифрованных данных
    QByteArray encryptedData = input.readAll();
    input.close();

    // Инициализация контекста дешифрования
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Инициализация дешифрования AES-256 CBC
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.data(), iv);

    // ИСПРАВЛЕНО: используем вектор вместо массива переменной длины
    std::vector<unsigned char> outBuffer(encryptedData.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen = 0;

    // Дешифрование данных
    EVP_DecryptUpdate(ctx, outBuffer.data(), &outLen,
                      (const unsigned char*)encryptedData.data(), encryptedData.size());

    int finalLen = 0;
    EVP_DecryptFinal_ex(ctx, outBuffer.data() + outLen, &finalLen);
    outLen += finalLen;

    // Запись дешифрованных данных
    output.write(reinterpret_cast<char*>(outBuffer.data()), outLen);
    output.close();

    EVP_CIPHER_CTX_free(ctx);
    QFile::remove(inputPath);
    qDebug() << "Decrypted:" << inputPath << "->" << outputPath;
}
