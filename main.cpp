#include <iostream>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFileInfo>
#include <string>
#include <Encryptor.h>
using namespace std;


int main()
{
    string raw_path;
    QTextStream out(stdout);
    out << "Input path: ";
    out.flush();  // Принудительно выводим сообщение
    cin >> raw_path; //Пример C:\qt15.0.1\projects\Lab_1
    QTextStream stream(stdout);
    QString path = QString::fromStdString(raw_path);//Преобразуем путь: String -> QString
    return 0;
}
