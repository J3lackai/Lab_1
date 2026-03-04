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
    QTextStream in(stdin);

    qInfo() << "Input path: ";
    QString path = in.readLine();

    qInfo() << "Input password: ";
    QString pswrd = in.readLine();
    qInfo() << "\nBefore decrypting, be sure to save a copy of the encrypted files,\n"
               "because if the password was entered incorrectly, the data will be permanently lost!!!\n";
    string mode;
    while(mode != "e" && mode != "d")
    {
        qInfo() << "Input 'e' for encode, 'd' for decode: ";
        QString Qmode = in.readLine();
        mode = Qmode.toStdString();
    }
    Encryptor& enc = Encryptor::getInstance();
    QString res;
    if (mode == "e")
        res = enc.encryptDirectory(path, pswrd) ? "\nComplete encrypt!" : "\nError encrypt!";
    else
        res = enc.decryptDirectory(path, pswrd) ? "\nComplete decrypt!" : "\nError decrypt!";
    qInfo() << res;
    return 0;
}
