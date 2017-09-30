#ifndef DATACALCULATOR_H
#define DATACALCULATOR_H

#include <QObject>
#include <QPair>
#include <QDate>
#include <QVariant>
#include <QString>
#include <QStringList>

class DataCalculator
{

public:
//    enum BiasType {Text, Number};
    enum DataType {Text, Number, Date};
    enum DateType {Week, Month};
    enum Counting {inCounting, byValue};

public:
    DataCalculator();

    void setDateData(QStringList dataList);
    void setDateType(DateType type);
    void setDateDataLimits(int min, int max);
    void filterByDate();

    void setKeyWords(QStringList list);
    void filterByKeyWords();

    void setAxisData(QStringList xList, QStringList yList);
    void setAxisDataType(DataType type);

    void setBiasValue(QVariant bias);
    void setBiasType(DataType type);
    void setBiasData(QStringList list);

    void setCountingMethod(Counting method);

    void calculate();

    QMap<QVariant, QPair<int, int>> getData();

private:
    QList<QDate>        dateData;
    int                 minDate;
    int                 maxDate;

    QStringList         xDataList;
    QStringList         yDataList;
    QStringList         biasDataList;

    QVariant            bias = NULL;

    QStringList         keyWords;

    DataType            biasType        = Number;
    DateType            dateType;
    Counting            countingMethod  = byValue;

    DataType            dataType        = Date;

    QMap<QVariant, QPair<int, int>> axisData;

};

#endif // DATACALCULATOR_H
