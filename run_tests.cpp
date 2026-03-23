#include <QDebug>
#include <string>
#include <TestEncryptor.h>
int main() {

    qDebug() << "Run Unit-tests...";

    int ret = 0;

    try {
        QTextStream in(stdin);

        qInfo() << "Input path: ";
        QString path = in.readLine();

        qInfo() << "Input password: ";
        QString pswrd = in.readLine();
        // Создаем экземпляр теста.
        TestUnitLogic testObj;

        // Запускаем логику тестирования
        testObj.runTests(path, pswrd);
        delete &testObj;
    } catch (...) {
        qDebug() << "Произошла ошибка во время выполнения теста.";
        ret = 1;
    }



    qDebug() << "Тестирование завершено. Код возврата:" << (ret == 0 ? "SUCCESS" : "FAILURE");

    return ret;
}
