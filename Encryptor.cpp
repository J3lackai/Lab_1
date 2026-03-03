#include <Encryptor.h>
#include <QDir>
#include <QDirIterator>
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
