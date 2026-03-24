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
        qDebug() << "Произошла ошибка во время выполнения теста.";
        ret = 1;
    }

    qDebug() << "Тестирование завершено. Код возврата:" << (ret == 0 ? "SUCCESS" : "FAILURE");

    return ret;
}
