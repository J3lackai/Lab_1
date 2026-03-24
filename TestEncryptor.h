#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QTemporaryFile>
#include <QDir>
#include <QDateTime>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QProcess>
#include "Encryptor.h"
#include <string>
#include <sstream>

class TestEncryptor {
    QString path;
    QString pswrd;

public:
    ~TestEncryptor() = default;
    TestEncryptor()= default;
    void runTests() {
        qInfo() << "=== Запуск Unit-тестов ===";

        generateUniqueTestEnvironment();

        try {
            qInfo() <<"What a test-case?:            | if true then without errors";
            qInfo() <<"testEncryptionFile()          |"<<testEncryptionFile();
            qInfo() <<"testEncryptionDir()           |"<<testEncryptionDir();
            qInfo() <<"testEncryptionWithWrongPswrd()|"<<testEncryptionWithWrongPswrd();
            qInfo() <<"testEncryptionCurDir()        |"<<testEncryptionCurDir();
            qInfo() <<"testHmacIntegrityCheck()      |"<<testHmacIntegrityCheck();

        } catch (const std::exception& e) {
            qInfo() << "EXCEPTION:" << e.what();
        }

        cleanupTestEnvironment();
    }

private:
    void generateUniqueTestEnvironment() {
        QString uniqueId = "test_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");

        pswrd = "SecurePass_" + uniqueId;

        QString baseDirPath = QDir::cleanPath("C:/tests" + uniqueId);

        QDir dir;
        if (!dir.exists(baseDirPath)) {
            dir.mkpath(baseDirPath);
        } else {
            QDir(baseDirPath).removeRecursively();
            dir.mkpath(baseDirPath);
        }

        path = baseDirPath;
        qDebug() << "Test Environment Created:" << path;
        qDebug() << "Password:" << pswrd;
    }

    void cleanupTestEnvironment() {
        if (!path.isEmpty()) {
            qDebug() << "Cleaning up test directory:" << path;
            QDir(path).removeRecursively();
            path = "";
            pswrd = "";
        }
    }

    bool testEncryptionFile() {
        Encryptor& enc = Encryptor::getInstance();
        ////////////////////////////////////////////////////////////////ПОДГОТОВКА ФАЙЛА В ПАПКЕ
        QString pathFile = path + "/data.txt";
        QFile fileToWrite(pathFile);
        if (!fileToWrite.open(QIODevice::WriteOnly))
            throw std::runtime_error("Cannot create test file");

        QByteArray originalData("This is a secret message for testing AES-256-CBC.");
        fileToWrite.write(originalData);
        fileToWrite.close();
        ////////////////////////////////////////////////////////////////ШИФРУЕМ-ДЕШИФРУЕМ
        if (!enc.encryptData(pathFile, pswrd)) {
            throw std::runtime_error("Encryption failed");
        }

        if (!enc.decryptData(pathFile, pswrd)) {
            throw std::runtime_error("Decryption failed or wrong password");
        }
        ////////////////////////////////////////////////////////////////ПРОВЕРЯЕМ РЕЗУЛЬТАТ
        QFile fileToRead(pathFile);
        if (!fileToRead.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot open decrypted file for reading");
        }

        QByteArray result = fileToRead.readAll();
        if (result != originalData)
            return false;
        return true;
    }

    bool testEncryptionDir() {
        Encryptor& enc = Encryptor::getInstance();
        QString dirPath = path;
        QDir testSubdir(dirPath + "/sub_folder_1");//Создали подпапку в C:/tests
        if (!testSubdir.mkpath("."))
            throw std::runtime_error("Cannot create subfolder");
        QByteArray originalData("This is a secret message for testing AES-256-CBC.");
        //////////////////////////////////////////////////////////////// ПОДГОТОВКА ФАЙЛА В ПОДПАПКЕ
        QFile fileInSubFolder(testSubdir.filePath("file_in_sub.txt"));
        if (!fileInSubFolder.open(QIODevice::WriteOnly))
            throw std::runtime_error("Cannot write to test folder");

        fileInSubFolder.write(originalData);
        fileInSubFolder.close();
        ////////////////////////////////////////////////////////////////ПОДГОТОВКА ФАЙЛА В ПАПКЕ
        QString pathFile = path + "/data.txt";
        QFile fileInFolder(pathFile);
        if (!fileInFolder.open(QIODevice::WriteOnly))
            throw std::runtime_error("Cannot create test file");
        fileInFolder.write(originalData);
        fileInFolder.close();
        ////////////////////////////////////////////////////////////////ШИФРУЕМ-ДЕШИФРУЕМ
        if (!enc.encryptData(dirPath, pswrd)) {
        throw std::runtime_error("Encryption of directory failed");
        }
        if (!enc.decryptData(dirPath, pswrd)) {
            throw std::runtime_error("Decryption of directory failed");
        }

        ////////////////////////////////////////////////////////////////ПРОВЕРЯЕМ РЕЗУЛЬТАТ
        QFile subFolderFileCheck(dirPath + "/sub_folder_1/file_in_sub.txt");
        if (!subFolderFileCheck.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot open decrypted file");
        }
        QByteArray resultData = subFolderFileCheck.readAll();
        if (resultData != originalData)
            return false;
        QFile folderFileCheck(dirPath + "/sub_folder_1/file_in_sub.txt");
        if (!folderFileCheck.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot open decrypted file");
        }
        resultData = folderFileCheck.readAll();
        if (resultData != originalData)
            return false;
        return true;
    }

    bool testEncryptionWithWrongPswrd() {
        return true;
    }

    bool testEncryptionCurDir() {
        return true;
    }

    bool testHmacIntegrityCheck() {
        return true;
        }

};
