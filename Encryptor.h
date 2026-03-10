#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QDir>
class Encryptor {
public:
    static Encryptor& getInstance();

    //методы для шифрования по пути
    bool encryptData(const QString& path, const QString& password);
    bool decryptData(const QString& path, const QString& password);

private:
    Encryptor() = default;
    //делаем конструктор приватным
    Encryptor(const Encryptor&) = delete;
    Encryptor& operator=(const Encryptor&) = delete;
    // запрещаем копирование и присвоение согласно паттерну Синглтон
    bool processFile(const QString& filePath, const QString& password, bool encrypt);
    QByteArray deriveKey(const QString& password);
    void encryptFile(const QString& filePath, const QByteArray& key);
    bool decryptFile(const QString& filePath, const QByteArray& key);
    bool traverseDirectory(const QString& pathFolder,
                           const QString& password, bool encrypt);
    // наработки из task1 будут здесь
};
#endif // ENCRYPTOR_H

