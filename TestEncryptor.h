#include <QObject>
#include <QTest> // Включаем заголовки Qt Test Framework (нужен в .pro)
#include <QStringList>
#include <QDebug>
#include <QTemporaryFile>
#include <QDir>
#include "Encryptor.h"
#include <string>
using namespace std;
class TestUnitLogic {
    QString path;
    QString pswrd;
    //делаем конструктор приватным
    TestUnitLogic(const TestUnitLogic&) = delete;
    TestUnitLogic& operator=(const TestUnitLogic&) = delete;
public:
    TestUnitLogic() = default;
    void runTests(QString path, QString pswrd) {
            qDebug() << "Запуск Unit-тестов...";

            if (path == NULL || pswrd == NULL)
                throw std::runtime_error("ERROR: path or pswrd == NULL");
            try {
                bool allpassed;
                testEncryptionFile(path, pswrd, allpassed);
                testEncryptionDir(path, pswrd, allpassed);
                testEncryptionCurDir( pswrd, allpassed);
                testEncryptionFileWithWrongPswrd(path, pswrd, allpassed);
                if (!allpassed) {
                    qDebug() << "\n=== ALL TESTS PASSED COMPLETE ===";
                } else {
                    qDebug() << "\n=== ERRORS DURING COMPLETE TESTS ===";
                }
            } catch (...) {
                qDebug() << "ERRORS DURING COMPLETE TESTS";
                // Если тест выбрасывает исключение, считаем его провалившимся (флаг passed останется false)
            }


        }

private:
    // Метод тестирования "туда-обратно" (шифрование -> дешифрировка)
        void testRoundEncryptionFile(QString encryptedPath, QString pswrd, bool&allpased) {
            Encryptor& enc = Encryptor::getInstance();

            QTemporaryFile srcFile;
            if (!srcFile.open()) throw std::runtime_error("Cannot open source file");

            QByteArray originalData("This is a secret message for testing AES-256-CBC.");
            srcFile.write(originalData);


            // Если encryptData вернет false, считаем тест провалившимся
            if (!enc.encryptData(encryptedPath, pswrd)) {
                throw std::runtime_error("Encryption failed");
            }

            QTemporaryFile destFile;
            // Мы не открываем этот файл сразу. Нам нужно создать путь для дешифрированного файла

            // Если decryptData вернет false, считаем тест провалившимся
            if (!enc.decryptData(encryptedPath, pswrd)) {
                throw std::runtime_error("Decryption failed or wrong password");
            } else {
                 // Дешифровка прошла успешно по флагу. Теперь проверяем данные вручную (так как QVERIFY может быть недоступен без QObject)

                 QFile fileToRead(encryptedPath);
                 if (!fileToRead.open(QIODevice::ReadOnly)) throw std::runtime_error("Cannot open decrypted file");

                 QByteArray result = fileToRead.readAll();

                 // Сравнение данных. Если данные не совпали, считаем тест провалившимся (устанавливаем флаг passed в false)
                 if (result != originalData) {
                     qDebug() << "Decrypted data mismatch!";
                     throw std::runtime_error("Decrypted content does not match original");
                 }

                 fileToRead.close();
            }

            srcFile.close(); // Очистка ресурсов источника, но не закрытого файла назначения (destFile)
        }
};

