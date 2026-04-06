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
    //9 тестов: 2 положительных; 7 негативных
    void generateUniqueTestEnvironment();
    void cleanupTestEnvironment();
    bool testEncryptionFile();
    bool testEncryptionDir(bool wrongpswrd = false); //Здесь 2 теста
    bool testEncryptionWrongPswrd();
    bool testEncryptionCurDir();
    bool testHmacIntegrityCheck();
    bool testPathNoExist();
    bool testEmptyPswrd();
    bool testHugePswrd();
    bool testNoEncryptLabel();
};
#endif
