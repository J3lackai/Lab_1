#include <QDebug>
#include <string>
#include <TestEncryptor.h>
int main() {

    qDebug() << "Run Unit-tests...";

    int ret = 0;

    try {
        QTextStream in(stdin);
        // Создаем экземпляр теста.
        TestEncryptor testObj;

        // Запускаем логику тестирования
        testObj.runTests();
    } catch (...) {
        qDebug() << "Error during passing tests.";
        ret = 1;
    }

    qDebug() << "All tests passed. Results:" << (ret == 0 ? "SUCCESS" : "FAILURE");

    return ret;
}
