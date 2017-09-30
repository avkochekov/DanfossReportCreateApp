#ifndef FILELOADER_H
#define FILELOADER_H

#include <QThread>
#include <QObject>
#include <QFile>
#include <QAxObject>

class FileLoader : public QThread
{

    Q_OBJECT

public:
    explicit FileLoader(QString fileName);
    void setColumnNumbers(QList<int>);
private:
    QList <int> readColumnList;

    void run();
    int totalElements = 0;
    int currentElement = 0;
    QString fileName;

    void readCSV(QString fileName, QStringList &titles, QList<QStringList> &data);
    void readEXCEL(QString fileName, QStringList &titles, QList<QStringList> &data);
public slots:
    void getProgress();
signals:
    void error(QString text);
    void tableData(int type,QStringList data);
    void readComplited();
    void readError();
    void progress(int percent);
};

#endif // FILELOADER_H
