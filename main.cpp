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
    string raw_path, raw_pswrd, mode;
    qInfo() << "Input path: ";
    cin >> raw_path;
    qInfo() << "Input password: ";
    cin >> raw_pswrd;
    qInfo() << "\nBefore decrypting, be sure to save a copy of the encrypted files,\n"
               "because if the password was entered incorrectly, the data will be permanently lost!!!\n";

    while(mode != "e" && mode != "d")
    {
        qInfo() << "Input 'e' for encode, 'd' for decode: ";
        cin >> mode;
    }
    QString path = QString::fromStdString(raw_path);//Преобразуем путь: String -> QString
    QString pswrd = QString::fromStdString(raw_pswrd);//Преобразуем пароль: String -> QString
    Encryptor& enc = Encryptor::getInstance();
    QString res;
    if (mode == "e")
        res = enc.encryptDirectory(path, pswrd) ? "\nComplete encrypt!" : "\nError encrypt!";
    else
        res = enc.decryptDirectory(path, pswrd) ? "\nComplete decrypt!" : "\nError decrypt!";
    qInfo() << res;
    return 0;
}
