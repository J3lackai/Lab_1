#include <QObject>
#include <QTest> // Включаем заголовки Qt Test Framework (нужен в .pro)
#include <QStringList>
#include <QDebug>
#include <QTemporaryFile>
#include <QDir>
#include "Encryptor.h"

class TestUnitLogic : public QObject { // Унаследовываемся от Q_OBJECT для работы с QTest
    Q_OBJECT

private slots:

    void initTestCase() {} // Инициализация перед запуском всех слотов (опционально)

    // Тест 1: Базовый цикл Шифрование -> Дешифровка
    void testRoundTripEncryption() {
        Encryptor& enc = Encryptor::getInstance();

        QString tempPath(QDir::tempPath()); // Исправлено: используем конструктор строки, а не qstringFromUtf8 (которого нет)

        QTemporaryFile srcFile;
        QVERIFY(srcFile.open());

        QByteArray originalData("This is a secret message for testing AES-256-CBC.");
        srcFile.write(originalData);

        QString encryptedPath = tempPath + "/test_encrypted.tmp"; // Просто конкатенация строк

        QVERIFY(enc.encryptData(encryptedPath, "SuperSecretPass123"));

        QTemporaryFile destFile;
        QVERIFY(destFile.open());

        QString decryptedPath = tempPath + "/test_decrypted.tmp";

        QVERIFY(enc.decryptData(decryptedPath, "SuperSecretPass123"));

        // Считываем результат и сравниваем с оригиналом
        QByteArray result;
        QFile decryptedResult(encryptedPath); // Открываем зашифрованный файл для чтения (чтобы проверить его) или создаем новый?
        // Логика теста: мы дешифрировали в *другой* файл, поэтому нужно читать именно его.

        QTemporaryFile finalDecrypted; // Создадим временный файл для результата дешифрации
        QVERIFY(finalDecrypted.open());

        QFile::copy(decryptedPath, finalDecrypted.fileName()); // Копируем из decryptedPath в наш новый файл? Нет...

        // Проще: открываем уже существующий файл decryptedPath и читаем его содержимое
        QFile fileToRead(decryptedPath);
        if (fileToRead.exists())
        {
            QVERIFY(fileToRead.open(QIODevice::ReadOnly));
            result = fileToRead.readAll();

            QCOMPARE(result, originalData); // Сравниваем с оригиналом

            fileToRead.close();
        }
        else
            QFAIL("Failed to open decrypted file");


        srcFile.close();
    }
};


// Функция для запуска тестов (стандартный Qt Test)
int main(int argc, char *argv[]) {

    int ret;

    // Инициализируем Qt Application для тестов (нужно хотя бы одно окно или процесс)
    QGuiApplication app(argc, argv);

    // Запускаем тесты через QTest::qExec
    TestUnitLogic* test = new TestUnitLogic();
    ret = QTest::qExec(test, argc, argv);

    delete test;
    return ret;
}
