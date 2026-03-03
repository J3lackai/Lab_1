#include <Encryptor.h>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
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
