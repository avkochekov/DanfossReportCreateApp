#ifndef PPTXWIDGET_H
#define PPTXWIDGET_H

#include <QWidget>
#include <QStringList>
#include <datacalculator.h>

namespace Ui {
class PptxWidget;
}

class PptxWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PptxWidget(QWidget *parent = 0);
    ~PptxWidget();

public slots:
    void    setCalculationsData(QList<QStringList> list);
    void    setOrdersData(QList<QStringList> list);
    void    makeWidgets();

private:
    void    makeDateLists();
    void    makeDateFilters();
    void    makeImage(int graphIndex);
    void    calculate();

private slots:
    void    createPptx();

private:
    enum GraphType{
        NewCalculations,
        CorrectionsOfCalculations,
        NewCalculationsAndCorrectionsByWeeks,
        NewCalculationsAndCorrectionsByMonths,
        ClosingReasons,
        CorrectionReasons,
        Orders};
    enum DataType {Text, Number, Date};
    enum Counting {inCounting, byValue};
    enum DateType{Week,Month};
    enum ReasonType{Correction,Closing};

    Ui::PptxWidget *ui;

    QMap<QVariant, QPair<int, int>> data;

    int xDataColumn;
    int yDataColumn;
    int biasColumn;
    int timeColumn;

    QString     graphTitle  = "";
    int         biasType    = DataCalculator::Number;
    int         dataType    = DataCalculator::Date;
    QVariant    biasValue   = 3;

    QList<QStringList>  CurrentData;
    QList<QStringList>  CalculationsData;
    QList<QStringList>  OrdersData;

    DateType            dateType    = Week;

    QList<int>          availableWeeks;
    QList<int>          availableMonths;
    int                 dateFrom;
    int                 dateTo;

    ReasonType          reasonType;
    QStringList         reasonsCorrection;
    QStringList         reasonsClosing;

    Counting            countingMethod  = byValue;

    QStringList         monthList;
    QStringList         graphTypes;

    // Для таблицы рассчетов
    // Столбец с датой
    const int CalculationDateColumn     = 13;
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
    const int TimeClosingColumn         = 25;
    // Адрес подразделения. Используется в графиках по дивизионам
    const int AddressColumn             = 00;

    // Для таблицы заказов
    // Столбец с датой
    const int OrdersDateColumn          = 12;
    // Количество задач
    const int NumberOfTasksColumn       = 17;
    // Наличие просрочки
    const int IsClosingColumn           = 19;

    int       DateColumn;
};

#endif // PPTXWIDGET_H
