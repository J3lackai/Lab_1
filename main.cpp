#include <iostream>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFileInfo>
#include <string>
#include <Encryptor.h>
using namespace std;

void print_attribs(QFileInfo& info, QTextStream& stream)
{//Функция вывода аттрибутов у файла или папки
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
    //Фунция выводит размер файла/папки
    qint64 i = 0;
    for (; nSize > 1023; nSize /= 1024, ++i) { }
    return QString().setNum(nSize) + "BKMGT"[i];
}

void print_all(const QString& path, QTextStream& stream) {
    //Функция решает подзадачу рекурсивного обхода папки по пути и вывода всех файлов и папок
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
               << info.lastModified().toString("dd.MM.yy") << " | ";
        print_attribs(info, stream);
        stream << "\n";
    }
}


int main()
{
    string raw_path;
    QTextStream out(stdout);
    out << "Input path: ";
    out.flush();  // Принудительно выводим сообщение
    cin >> raw_path; //Пример C:\qt15.0.1\projects\Lab_1
    QTextStream stream(stdout);
    QString path = QString::fromStdString(raw_path);//Преобразуем путь: String -> QString
    print_all(path,stream);
    return 0;
}
