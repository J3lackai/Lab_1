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
            qInfo() <<"What a test-case?:        | if true then without errors\n";
            qInfo() <<"testEncryptionFile()      | "<<testEncryptionFile()<<"\n";
            qInfo() <<"testEncryptionDir()       | "<<testEncryptionDir()<<"\n";
            qInfo() <<"testEncryptionWrongPswrd()| "<<testEncryptionWrongPswrd()<<"\n";
            qInfo() <<"testEncryptionCurDir()    | "<<testEncryptionCurDir()<<"\n";
            qInfo() <<"testHmacIntegrityCheck()  | "<<testHmacIntegrityCheck()<<"\n";

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
            return false;
        }

        if (!enc.decryptData(pathFile, pswrd)) {
            return false;
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

    bool testEncryptionDir(bool wrongpswrd = false) {
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
            return false;
        }
        if (wrongpswrd)
            return enc.decryptData(dirPath, QString("1"));//Правильный пароль начинается с SecurePass...
        if (!enc.decryptData(dirPath, pswrd)) {
            return false;
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

    bool testEncryptionWrongPswrd() {
            return !testEncryptionDir(true); // Ожидаем что защита сработает encryptData вернёт false
        }

    bool testEncryptionCurDir() {
        Encryptor& enc = Encryptor::getInstance();
        QString cur_path = QDir::current().path();
        qDebug() << "Current Project Path:" << cur_path;

        return !enc.encryptData(cur_path, pswrd);// Ожидаем что защита сработает encryptData вернёт false
    }

    bool testHmacIntegrityCheck() {
        Encryptor& enc = Encryptor::getInstance();
        ////////////////////////////////////////////////////////////////ПОДГОТОВКА ФАЙЛА В ПАПКЕ
        QString pathFile = path + "/data.txt";
        QFile fileToWrite(pathFile);
        if (!fileToWrite.open(QIODevice::WriteOnly))
            throw std::runtime_error("Cannot create test file");
        QByteArray originalData("This is a secret message for testing AES-256-CBC.");
        fileToWrite.write(originalData);
        fileToWrite.close();
        ////////////////////////////////////////////////////////////////ШИФРУЕМ
        if (!enc.encryptData(pathFile, pswrd)) {
            return false;
        }
        ////////////////////////////////////////////////////////////////ПОРТИМ СОДЕРЖИМОЕ
        QFile fileToDamage(pathFile);
        if (!fileToDamage.open(QIODevice::WriteOnly))
            throw std::runtime_error("Cannot open encrypted file");
        QByteArray wrongData("ia isportil polzovatelskie dannie haha");
        fileToDamage.write(wrongData);
        fileToDamage.close();
        ////////////////////////////////////////////////////////////////ДЕШИФРУЕМ
        return !enc.decryptData(pathFile, pswrd); // Ожидаем что защита сработает decryptData вернёт false
    }

};
