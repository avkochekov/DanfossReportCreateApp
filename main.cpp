#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    #ifdef Q_OS_WIN32
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
    #endif

    #ifdef Q_OS_LINUX
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    #endif

    QTranslator myTranslator;
    myTranslator.load(":/RUS");
    a.installTranslator(&myTranslator);

    MainWindow w;
    w.setWindowTitle("Danfoss Graph Generator");
    w.setWindowIcon(QIcon(":/logo.ico"));
    w.show();

    return a.exec();
}
