#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDate>
#include <graph.h>
#include <graphwidget.h>
#include <pptxwidget.h>
#include <fileloader.h>

typedef QList<QStringList> QStrringLists;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    enum Filtration{Weeks,Month};

    GraphWidget *graphWidget;
    PptxWidget  *pptxWidget;


    Ui::MainWindow *ui;

    QStringList         dataTitles;
    QList<QStringList>  dataValues;

    QList<QStringList>  CalculationsDatas;
    QList<QStringList>  OrdersDatas;

    QStringList  CalculationsDataTitles;
    QStringList  OrdersDataTitles;

    QString             fileName;

    int blockOpenButton = 0;

    int WeeksColumn  = 17;
    int MonthColumn  = 13;
    int filterColumn = 18;
    int AtticReasonColumn = 20;
    int AdjustReasonColumn = 21;
    int ValueColumn  = 0;
    int KeyColumn    = 13;
    int Divisions    = 0;

    QStringList reasonList;

    int timeRangeFilter = 17;
    int timeRangeIndex = 0;
    int divisionIndex = -1;

    QString xTitle = "Номер недели";
    QString yTitle;
    QString graphTitle;

    QTranslator *translator;

    int indexGraph      = 0;
    int indexMonthFrom  = 0;
    int indexMonthTo    = 0;

    void sortByDate(int column);
    QList<QVariant> selectWeeks(QList<QString> dataIn);
    QList<QVariant> selectMonth(QList<QString> dataIn);
    QList<QVariant> selectReasons(QList<QString> dataIn);

    QProgressBar *progressBar = new QProgressBar;
private slots:
    void    openFile(QString fileName, QStringList &titles, QList<QStringList> &datas, QPushButton *button, QList<int> list);
    void    fillData(int type, QStringList data);
//    Graph *generateTwoBarsData(QStringList xDataList, QStringList yDataList);
//    Graph *generateHorizontalBarsData(QStringList xDataList, QStringList yDataList);

    void    saveGraph();

    void    getMessage(QString);

    void    setLocale(QString locale);

signals:
    void    replotGraph();
    void    opened();
    void    localeChanged(QString locale);
};

//class DTableWidgetItem : public QTableWidgetItem
//{
//public:
//    explicit DTableWidgetItem(const QString &text, int type = Type);
//    bool operator< (const QTableWidgetItem&  other) const;

//};

//class WTableWidgetItem : public QTableWidgetItem
//{
//public:
//    explicit WTableWidgetItem(const QString &text, int type = Type);
//    bool operator< (const QTableWidgetItem&  other) const;

//};

#endif // MAINWINDOW_H
