#include "fileloader.h"
#include <qt_windows.h>

FileLoader::FileLoader(QString _fileName)
{
    fileName = _fileName;
}

void FileLoader::setColumnNumbers(QList<int> list)
{
    readColumnList = list;
}

void FileLoader::run()
{
    QStringList        tableTitles;
    QList<QStringList> tableValues;

    QString fileNameSuffix = fileName.split(".").last();
    if (fileNameSuffix == "xlsx" || fileNameSuffix == "xls")
        readEXCEL(fileName, tableTitles, tableValues);
    if (fileNameSuffix == "csv")
        readCSV(fileName, tableTitles, tableValues);

    if (tableTitles.isEmpty() || tableValues.isEmpty()){
        emit readError();
        return;
    }
    emit tableData(0,tableTitles);
    foreach (QStringList values, tableValues) {
        emit tableData(1,values);
    }
    emit readComplited();
//    emit readComplited(tableTitles, tableValues);
}

void FileLoader::readCSV(QString fileName, QStringList &titles, QList<QStringList> &data)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        fileName.clear();
        return;
    }

    QStringList strings = QString(file.readAll()).split("\n");
    totalElements = strings.size();

    if (totalElements != 0){


        titles = QString(strings.at(0)).simplified().split(";");
        currentElement++;
        getProgress();

        strings.removeFirst();

        foreach (QString string, strings){
            if (!string.isEmpty()){
                QStringList dataList = string.split(";");
                for (int i=0; i<dataList.size(); i++){
                    data[i].append(dataList.at(i));
                }
            }
            currentElement++;
            getProgress();
        }
    }
    file.close();
}

void FileLoader::readEXCEL(QString fileName, QStringList &titles, QList<QStringList> &data)
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    QAxObject* excel = new QAxObject("Excel.Application", this);
    excel->setProperty("DisplayAlerts", false);
    excel->setProperty("Visible", false);
    QAxObject *workbooks=excel->querySubObject("Workbooks");

    //Open docxFile
    QString filename(fileName);
    QVariant confirmconversions(false);
    QVariant readonly(true);
    QVariant addtorecentfiles(false);
    QVariant passworddocument("");
    QVariant passwordtemplate("");
    QVariant revert(false);

    QAxObject* workbook = workbooks->querySubObject("Open(const QVariant&, const QVariant&, const QVariant&",
                                                    filename,
                                                    readonly,
                                                    confirmconversions);
//    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", fileName);
    if (workbook == 0){
        emit error(tr("Невозможно открыть документ. \n"
                   "Файл не существует или поврежден."));
        return;
    }
    // проверка открытия файла!!!
    QAxObject* sheets = workbook->querySubObject("Worksheets");
    QAxObject* sheet = sheets->querySubObject("Item(int)", 2);
    // получение количества используемых строк и столбцов документа
    QAxObject* usedRange = sheet->querySubObject("UsedRange");
    QAxObject* rows = usedRange->querySubObject("Rows");
    QAxObject* columns = usedRange->querySubObject("Columns");

    int countRows = rows->property("Count").toInt();
    int countCols = columns->property("Count").toInt();
    // номер ячейки - начало таблицы с данными
    int startRow = 179;
//    totalElements = (countRows - startRow) * countCols;
    totalElements = (countRows - startRow) * readColumnList.size();

    if (totalElements != 0){
        // чтение данных из файла
        // заголовки столбцов
        for (int column = 0; column < countCols; column++){
            QAxObject* cell = sheet->querySubObject("Cells(int,int)", startRow, column + 1);
            titles.append(cell->dynamicCall("value").toString());
            currentElement++;
            getProgress();
        }
        // значения столбцов
//        for (int column = 0; column < countCols; column++){
        for (int column = 0; column < countCols; column++){
            data.append(QStringList());
            if (readColumnList.indexOf(column) == -1)
                continue;
            for (int row = startRow; row < countRows; row++){
                QAxObject* cell = sheet->querySubObject("Cells(int,int)", row + 1, column + 1);
                data.last().append(cell->dynamicCall("value").toString());
                currentElement++;
                getProgress();
            }
        }
    }

    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
}

void FileLoader::getProgress()
{
    if (totalElements == 0)
        return;
    int percent = ((double)currentElement / (double)totalElements) * 100;
    emit progress(percent);
}
