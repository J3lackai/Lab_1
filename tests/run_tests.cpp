#include <QDebug>
#include <string>
#include <QProcess>
#include <tests/TestEncryptor.h>
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
    qInfo() << "Test Environment Created:" << path;
    qInfo() << "Password:" << pswrd;
}
void TestEncryptor::cleanupTestEnvironment() {
    if (!path.isEmpty()) {
        qInfo() << "Cleaning up test directory:" << path;
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
        qInfo() <<"testNoEncryptLabel()      | "<<testNoEncryptLabel()<<"\n";


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
    qInfo() << "Current Project Path:" << cur_path;
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
    qInfo() << "Current Project Path:" << cur_path;
    return !enc.encryptData(cur_path, pswrd);
}
bool TestEncryptor::testEmptyPswrd()
{
    Encryptor& enc = Encryptor::getInstance();
    QString cur_pswrd = QString(""); //Записали пустой пароль
    qInfo() << "Current Pswrd: ''";
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
bool TestEncryptor::testNoEncryptLabel() {
    Encryptor& enc = Encryptor::getInstance();

    // Создаем структуру папок
    QString subFolder1Path = path + "/sub_folder_1";
    QString subFolder2Path = path + "/sub_folder_2";

    QDir subFolder1(subFolder1Path);
    QDir subFolder2(subFolder2Path);

    // Создаем обе папки
    if (!subFolder1.mkpath("."))
        throw std::runtime_error("Cannot create sub_folder_1");
    if (!subFolder2.mkpath("."))
        throw std::runtime_error("Cannot create sub_folder_2");

    // Создаем тестовый файл в sub_folder_2
    QString actualFilePath = subFolder2Path + "/test_data_in_sub_folder_2.txt";
    QFile testFileInSub2(actualFilePath);
    if (!testFileInSub2.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Cannot create test file in sub_folder_2");
    }

    QByteArray originalData("This is secret data that should not be encrypted via shortcut");
    testFileInSub2.write(originalData);
    testFileInSub2.close();

    // СОЗДАЕМ .LNK ЯРЛЫК через PowerShell
    QString shortcutPath = subFolder1Path + "/link_to_sub_folder_2.lnk";
    // PowerShell скрипт для создания ярлыка
    QString psScript = QString(
        "$WScriptShell = New-Object -ComObject WScript.Shell; "
        "$Shortcut = $WScriptShell.CreateShortcut('%1'); "
        "$Shortcut.TargetPath = '%2'; "
        "$Shortcut.Save();"
    ).arg(QDir::toNativeSeparators(shortcutPath))
     .arg(QDir::toNativeSeparators(subFolder2Path));

    QProcess process;
    process.start("powershell.exe", QStringList() << "-Command" << psScript);
    process.waitForFinished();

    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        qWarning() << "Failed to create .lnk shortcut:" << error;
        return false;
    }

    qInfo() << "Created .lnk shortcut via PowerShell:" << shortcutPath;

    // Проверяем, что ярлык создался
    if (!QFile::exists(shortcutPath)) {
        qWarning() << "Shortcut was not created";
        return false;
    }

    QFileInfo shortcutInfo(shortcutPath);
    qInfo() << "Shortcut size:" << shortcutInfo.size();

    // Шифруем папку sub_folder_1
    qInfo() << "Encrypting sub_folder_1...";
    if (!enc.encryptData(subFolder1Path, pswrd)) {
        qWarning() << "Failed to encrypt sub_folder_1";
        return false;
    }

    // Проверяем, что файл в sub_folder_2 остался не зашифрован
    QFile checkFileInSub2(actualFilePath);
    if (!checkFileInSub2.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file in sub_folder_2 after encryption";
        return false;
    }

    QByteArray resultData = checkFileInSub2.readAll();
    checkFileInSub2.close();

    if (resultData != originalData) {
        qWarning() << "Data in sub_folder_2 was modified after encrypting sub_folder_1";
        return false;
    }

    qInfo() << "Data in sub_folder_2 remains untouched after encryption";

    // Расшифровываем папку sub_folder_1
    qInfo() << "Decrypting sub_folder_1...";
    if (!enc.decryptData(subFolder1Path, pswrd)) {
        qWarning() << "Failed to decrypt sub_folder_1";
        return false;
    }

    qInfo() << "Successfully decrypted sub_folder_1";

    // Файл в sub_folder_2 все еще должен быть доступен
    QFile finalCheckFile(actualFilePath);
    if (!finalCheckFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file in sub_folder_2 after decryption";
        return false;
    }

    QByteArray finalData = finalCheckFile.readAll();
    finalCheckFile.close();

    if (finalData != originalData) {
        qWarning() << "Data in sub_folder_2 was corrupted after decryption";
        return false;
    }

    qInfo() << "All checks passed: sub_folder_2 was not encrypted via .lnk shortcut ";
    return true;
}
/*
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
*/
