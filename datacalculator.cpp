#include "datacalculator.h"

DataCalculator::DataCalculator()
{

}

void DataCalculator::setDateData(QStringList dataList){
    foreach (QString data, dataList)
        dateData.append(QDate::fromString(data.remove(QRegExp("\\T.*")),"yyyy-M-d"));
}

void DataCalculator::setDateType(DataCalculator::DateType type)
{
    dateType = type;
}

void DataCalculator::setDateDataLimits(int min, int max){
    minDate = min;
    maxDate = max;
}

void DataCalculator::filterByDate(){
    int position = 0;
    foreach (QDate date, dateData) {
        int currentDate;
        switch (dateType) {
        case Month:
            currentDate = date.month();
            break;
        case Week:
            currentDate = date.addDays(1).weekNumber();
            break;
        }
        if (currentDate < minDate || currentDate > maxDate){
            dateData        .removeAt(position);
            biasDataList    .removeAt(position);
            xDataList       .removeAt(position);
            yDataList       .removeAt(position);

        } else {
            position++;
        }
    }
}

void DataCalculator::setKeyWords(QStringList list){
    keyWords = list;
}

void DataCalculator::filterByKeyWords()
{
    if (keyWords.isEmpty())
        return;
    int position = 0;
    foreach (QString data, xDataList) {
        if (keyWords.indexOf(data) < 0){
            dateData        .removeAt(position);
            biasDataList    .removeAt(position);
            xDataList       .removeAt(position);
            yDataList       .removeAt(position);
        } else {
            position++;
        }
    }
}

void DataCalculator::setAxisData(QStringList xList, QStringList yList){
    xDataList = xList;
    yDataList = yList;
}

void DataCalculator::setAxisDataType(DataCalculator::DataType type)
{
    dataType = type;
}

void DataCalculator::setBiasValue(QVariant bias){
    this->bias = bias;
}

void DataCalculator::setBiasType(DataCalculator::DataType type)
{
    biasType = type;
}

void DataCalculator::setBiasData(QStringList list)
{
    biasDataList = list;
}

void DataCalculator::setCountingMethod(Counting method){
    countingMethod = method;
}


void DataCalculator::calculate()
{
    QVariant biasDataValue;
    for (int i = 0; i < xDataList.size(); i++){
        if (xDataList.at(i).isEmpty() || yDataList.at(i).isEmpty())
            continue;

        QVariant xDataValue;
        switch (dataType) {
        case Text:
            xDataValue = xDataList.at(i);
            break;
        case Date:
            QDate date = QDate::fromString(xDataList.at(i),Qt::ISODate).addDays(1);
            switch (dateType) {
            case Month:
                xDataValue = date.month();
                break;
            case Week:
                xDataValue = date.weekNumber();
                break;
            }
            break;
        }

        int     yDataValue = 0;

        switch (countingMethod) {
        case inCounting:
            yDataValue = 1;
            break;
        case byValue:
            yDataValue = QString(yDataList.at(i)).remove(QRegExp("\\D")).toInt();;
            break;
        }

        if (!bias.isNull() ){
            switch (biasType) {
            case Text:
                biasDataValue = biasDataList.at(i);
                if (biasDataValue != bias){
                    axisData[xDataValue].first += yDataValue;
                } else {
                    axisData[xDataValue].second += yDataValue;
                }
                break;
            case Number:
                biasDataValue = QString(biasDataList.at(i)).remove(QRegExp("\\D")).toInt();;
                if (biasDataValue <= bias){
                    axisData[xDataValue].first += yDataValue;
                } else {
                    axisData[xDataValue].second += yDataValue;
                }
                break;
            }
        } else {
            axisData[xDataValue].first += yDataValue;
        }
    }
}

QMap<QVariant, QPair<int, int> > DataCalculator::getData()
{
    return axisData;
}
