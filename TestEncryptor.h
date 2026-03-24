#ifndef TestEncryptor_H
#define TestEncryptor_H
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include "Encryptor.h"

class TestEncryptor {
    QString path;
    QString pswrd;

public:
    ~TestEncryptor() = default;
    TestEncryptor()= default;
    void runTests();
private:
    void generateUniqueTestEnvironment();
    void cleanupTestEnvironment();
    bool testEncryptionFile();
    bool testEncryptionDir(bool wrongpswrd = false);
    bool testEncryptionWrongPswrd();
    bool testEncryptionCurDir();
    bool testHmacIntegrityCheck();

};
#endif
