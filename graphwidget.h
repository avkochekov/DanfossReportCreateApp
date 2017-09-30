#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QComboBox>

namespace Ui {
class GraphWidget;
}

class GraphWidget : public QWidget
{
    Q_OBJECT

private:
    enum DataType {Text, Number, Date};
    enum Counting {inCounting, byValue};
    enum DateType{Week,Month};
    enum ReasonType{Correction,Closing};
    enum GraphType{NONE, NewCalculations, CorrectionsOfCalculations, NewCalculationsAndCorrections, ClosingReasons, CorrectionReasons, ByDivisions, Orders};



public:
    explicit GraphWidget(QWidget *parent = 0);
    ~GraphWidget();

public slots:
    void    setCalculationsData(QList<QStringList> list);
    void    setOrdersData(QList<QStringList> list);
    void    setLocale(QString locale);
    void    makeWidgets();


private:
    void    reloadLists();
    void    reloadWidgets();
    void    reloadComboBox(QComboBox *widget, QStringList list);
    void    reloadComboBox(QComboBox *widget, QList<int>  list);
    void    makeDateLists(QList<QString> dataIn);
    void    makeReasonLists();
    void    pickReasonByDate(QStringList inList, QStringList &outList);
    void    adjustTimeFilter();

    QList<QVariant> selectReasons(QList<QString> dataIn);

private slots:
    void    setGraphType(int type);
    void    setFilterType(int type);
    void    setCountingMethod(int index);
    void    setDivision(int index);

    void    changeWidgetsVisibility();
    void    changeDataSources();

    void    makeDateFilters();
    void    makeReasonFilters();

    void    checkDates(int index);

    void    openGraph();

private:
    Ui::GraphWidget *ui;

    // Основные списки с данными (по колонкам)
    QList<QStringList>  CurrentData;
    QList<QStringList>  CalculationsData;
    QList<QStringList>  OrdersData;

    GraphType           graphType;

    DateType            dateType    = Week;
    QList<int>          availableMonths;
    QList<int>          availableWeeks;
    int                 dateFrom;
    int                 dateTo;

    ReasonType          reasonType;
    QStringList         reasonsCorrection;
    QStringList         reasonsClosing;

    Counting            countingMethod;

    int xDataColumn;
    int yDataColumn;
    int biasColumn;
    int timeColumn;
    int divisionIndex   = 0;

    // Для таблицы рассчетов
    // Столбец с датой
    const int CalculationMonthColumn    = 13;
    const int CalculationWeekColumn     = 17;
    // Расчеты БТП
    const int NewCalculationsColumn     = 37;
    // Корректировки БТП
    const int CorrectionsColumn         = 38;
    // Расчеты и корректировки БТП (сумма)
    const int CalcAndCorrectionsColumn  = 39;
    // Причины корректировок и просрочек
    const int ClosingReasonColumn       = 20;
    const int CorrectionReasonColumn    = 21;
    // Время закрытия задачи
    const int TimeClosingColumn         = 18;
    // Адрес подразделения. Используется в графиках по дивизионам
    const int AddressColumn             = 00;



    // Для таблицы заказов
    // Столбец с датой
    const int OrdersMonthColumn         = 12;
    const int OrdersWeekColumn          = 15;
    // Количество задач
    const int NumberOfTasksColumn       = 17;
    // Наличие просрочки
    const int IsClosingColumn           = 19;


//    // Номера колонок с Причинами просроченных задач и Причинами корректировок
//    int OverdueReasonColumn;
//    int CorrectionReasonColumn;

    QStringList         graphTypes;
    QStringList         monthList;
    QList<QStringList>  divisionsList;
    QStringList         dataTitles;

    int                 WeekColumn;
    int                 MonthColumn;
    int                 DateColumn;

signals:
    void    graphTypeChanged();
    void    filterTypeChanged();
};

#endif // GRAPHWIDGET_H
