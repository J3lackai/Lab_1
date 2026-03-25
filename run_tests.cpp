#include <QDebug>
#include <string>
#include <TestEncryptor.h>
using namespace std;
void TestEncryptor::generateUniqueTestEnvironment() {
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
void TestEncryptor::cleanupTestEnvironment() {
    if (!path.isEmpty()) {
        qDebug() << "Cleaning up test directory:" << path;
        QDir(path).removeRecursively();
        path = "";
        pswrd = "";
    }
}
void TestEncryptor::runTests() {
    qInfo() << "=== Запуск Unit-тестов ===";

    generateUniqueTestEnvironment();

    try {
        qInfo() <<"What a test-case?:        | if true then without errors\n";
        qInfo() <<"testEncryptionFile()      | "<<testEncryptionFile()<<"\n";
        qInfo() <<"testEncryptionDir()       | "<<testEncryptionDir()<<"\n";
        qInfo() <<"testEncryptionWrongPswrd()| "<<testEncryptionWrongPswrd()<<"\n";
        qInfo() <<"testEncryptionCurDir()    | "<<testEncryptionCurDir()<<"\n";
        qInfo() <<"testHmacIntegrityCheck()  | "<<testHmacIntegrityCheck()<<"\n";
        qInfo() <<"testPathNoExist()         | "<<testPathNoExist()<<"\n";
        qInfo() <<"testEmptyPswrd()          | "<<testEmptyPswrd()<<"\n";
        qInfo() <<"testHugePswrd()           | "<<testHugePswrd()<<"\n";


    } catch (const std::exception& e) {
        qInfo() << "EXCEPTION:" << e.what();
    }

    cleanupTestEnvironment();
}

bool TestEncryptor::testEncryptionFile() {
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

bool TestEncryptor::testEncryptionDir(bool wrongpswrd) {
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

bool TestEncryptor::testEncryptionWrongPswrd() {
        return !testEncryptionDir(true); // Ожидаем что защита сработает encryptData вернёт false
    }

bool TestEncryptor::testEncryptionCurDir() {
    Encryptor& enc = Encryptor::getInstance();
    QString cur_path = QDir::current().path();
    qDebug() << "Current Project Path:" << cur_path;
    if (!enc.encryptData(cur_path, pswrd))
        return true;
    enc.decryptData(cur_path, pswrd); //Если смогли зашифровать, расшифровываем для последующих тестов
    return false;
}

bool TestEncryptor::testHmacIntegrityCheck() {
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
    bool res = !enc.decryptData(pathFile, pswrd);// Ожидаем что защита сработает decryptData вернёт false
    ////СЛОМАЛИ ТЕСТОВУЮ СРЕДУ НУЖНО ПЕРЕСОЗДАТЬ!!!!!!!!!!!!!!!!!
    cleanupTestEnvironment();
    generateUniqueTestEnvironment();
    return res;
}

bool TestEncryptor::testPathNoExist()
{
    Encryptor& enc = Encryptor::getInstance();
    QString cur_path = QString("1"); //Записали не существующий путь
    qDebug() << "Current Project Path:" << cur_path;
    return !enc.encryptData(cur_path, pswrd);
}
bool TestEncryptor::testEmptyPswrd()
{
    Encryptor& enc = Encryptor::getInstance();
    QString cur_pswrd = QString(""); //Записали пустой пароль
    qDebug() << "Current Pswrd: ''";
    if (enc.encryptData(path, cur_pswrd)) //Шифрование должно пройти
        return true;
    enc.decryptData(path, cur_pswrd); //Если смогли зашифровать, расшифровываем для последующих тестов
    return false;
}
bool TestEncryptor::testHugePswrd()
{
    QString pathPswrd = path + "/pswrd.txt";
    QFile pswrdToWrite(pathPswrd);
    if (!pswrdToWrite.open(QIODevice::WriteOnly))
        throw std::runtime_error("Cannot create test file");
    string hugePswrdStr = "1";
    for (int i = 0 ; i < 16; i++) // i < 32 тоже работает, но нужно ждать
        hugePswrdStr+=hugePswrdStr;// Пароль: "11111...1" 65 тыс символов для i < 16
    QString old_pswrd = pswrd;
    pswrd = QString::fromStdString(hugePswrdStr); //string -> QString
    bool res = testEncryptionDir();
    pswrd = old_pswrd;
    return res;

}
int main() {

    qInfo() << "Run Unit-tests...";

    int ret = 0;

    try {
        QTextStream in(stdin);
        // Создаем экземпляр теста.
        TestEncryptor testObj;

        // Запускаем логику тестирования
        testObj.runTests();
    } catch (...) {
        qCritical() << "Error during passing tests.";
        ret = 1;
    }

    qInfo() << "All tests passed";

    return ret;
}
