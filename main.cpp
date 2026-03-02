#include <iostream>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFileInfo>
using namespace std;
void print_attribs(QFileInfo& info, QTextStream& stream) {
    if (info.isReadable())
        stream << "R";
    if (info.isWritable())
        stream << "W";
    if (info.isHidden())
        stream << "H";
    if (info.isExecutable())
        stream << "E";
}

QString fileSize(qint64 nSize) {
    qint64 i = 0;
    for (; nSize > 1023; nSize /= 1024, ++i) { }
    return QString().setNum(nSize) + "BKMGT"[i];
}
void print_files(QString path, QTextStream& stream)
{
    if (!QDir(path).exists())
    {
        qWarning() << "Directory does not exist:" << path;
        return;
    }
    QDirIterator itFiles(path, QDir::Files);
    while (itFiles.hasNext()) {
        itFiles.next();

        QFileInfo info = itFiles.fileInfo();
        stream << "F | " << info.fileName() << " | " << fileSize(info.size()) << " | "
               << info.lastModified().toString() << " | ";
        print_attribs(info, stream);
        stream << "\n";
    }
}
void print_all(QString path, QTextStream& stream) {
    if (!QDir(path).exists())
    {
        qWarning() << "Directory does not exist:" << path;
        return;
    }
    QDirIterator itDirs(path, QDir::Dirs);

    while (itDirs.hasNext()) {
        itDirs.next();

        if (itDirs.fileName() == ".")
            continue;

        QFileInfo info = itDirs.fileInfo();
        stream << "D | " << info.fileName() << " | " << fileSize(info.size()) << " | "
               << info.lastModified().toString() << " | ";
        print_attribs(info, stream);
        stream << "\n";
    }
    print_files(path,stream);


}


int main()
{
    QString path("C:\\qt15.0.1\\projects\\Lab_1");
    QTextStream stream(stdout);
    print_all(path,stream);
    cout << "Complete!";
    return 0;
}
