#include <iostream>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFileInfo>
#include <string>
#include <QDebug>
#include <QTextStream>
#include <Encryptor.cpp>

using namespace std;


int main()
{
    string raw_path, raw_pswrd;
    qInfo() << "Input path: ";
    cin >> raw_path;
    qInfo() << "Input password: ";
    cin >> raw_pswrd;
    QString path = QString::fromStdString(raw_path);//Преобразуем путь: String -> QString
    QString pswrd = QString::fromStdString(raw_pswrd);//Преобразуем пароль: String -> QString
    Encryptor& enc = Encryptor::getInstance();
    QString res = enc.encryptDirectory(path, pswrd) ? "\nComplete encrypt!" : "\nError encrypt!";
    qInfo() << res;
    return 0;
}
