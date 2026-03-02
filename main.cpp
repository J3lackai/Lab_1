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

void print_all(const QString& path, QTextStream& stream) {
    if (!QDir(path).exists())
    {
        qWarning() << "Directory does not exist:" << path;
        return;
    }
    QDirIterator itDirs(path,
            QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (itDirs.hasNext()) {
        QFileInfo info(itDirs.next());
        stream << (info.isDir() ? "D" : "F") << " | "
               << info.filePath() << " | "
               << fileSize(info.size()) << " | "
               << info.lastModified().toString("dd.MM.yyyy hh:mm:ss") << " | ";
        print_attribs(info, stream);
        stream << "\n";
    }
}


int main()
{
    QString path("C:\\qt15.0.1\\projects\\Lab_1");
    QTextStream stream(stdout);
    print_all(path,stream);
    cout << "Complete!";
    return 0;
}
