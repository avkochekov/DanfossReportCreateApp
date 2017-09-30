#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCalendarWidget>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include <algorithm>
#include <QPair>
#include <QThread>
#include <QMessageBox>
#include <QDebug>

#include <QAxObject>
#include <QtGui>

#include <QFileDialog>

#include <graph.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    translator = new QTranslator();
    connect(ui->actionEnglish, &QAction::triggered, [this](){
        setLocale(":/ENG");
        emit localeChanged(":/ENG");
    });
    connect(ui->actionRussian, &QAction::triggered, [this](){
        setLocale(":/RUS");
        emit localeChanged(":/RUS");
    });
    setLocale(":/RUS");

    this->setFixedWidth(this->width());
    this->setFixedHeight(this->height());
    this->setGeometry(20,20,this->width(),this->height());

    graphWidget = new GraphWidget();
    graphWidget->setWindowTitle(tr("Создать график"));

    pptxWidget = new PptxWidget();
    pptxWidget->setWindowTitle(tr("Создать график"));

    progressBar->setTextVisible(true);

    connect(ui->openCalculations, &QPushButton::clicked, [&](){
        QString fileName = QFileDialog::getOpenFileName(this,
                                            tr("Открыть файл"),
                                            QDir::currentPath(),
                                            "Excel (*.xlsx *.xls)");

        // Обязательна привязка к столбцу 17, т.к. CRM выгружает какую-то херню

        if (!fileName.isEmpty()){
            QList<int> calculationsColumnList = {0, 13, 18, 20, 21, 37, 38, 39};
            ui->openCalculationsPath->setText(fileName);
            openFile(fileName, CalculationsDataTitles, CalculationsDatas, ui->openCalculations, calculationsColumnList);
        }
    });
    connect(ui->openOrders, &QPushButton::clicked, [&](){
        QString fileName = QFileDialog::getOpenFileName(this,
                                            tr("Открыть файл"),
                                            QDir::currentPath(),
                                            "Excel (*.xlsx *.xls)");
        if (!fileName.isEmpty()){
            QList<int> ordersColumnList = {12, 15, 17, 19};
            ui->openOrdersPath->setText(fileName);
            openFile(fileName, OrdersDataTitles, OrdersDatas, ui->openOrders, ordersColumnList);
        }
    });
    connect(ui->createGraph,        &QPushButton::clicked, graphWidget, &GraphWidget::show);
    connect(ui->createPresentation, &QPushButton::clicked, pptxWidget, &PptxWidget::show);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::openFile(QString fileName, QStringList &titles, QList<QStringList> &datas, QPushButton *button, QList<int> list)
{
    titles.clear();
    datas.clear();
    FileLoader *loader = new FileLoader(fileName);
    loader->setColumnNumbers(list);
    button->blockSignals(true);

    connect(loader, &FileLoader::started,   [&, button](){
        button->setText(tr("В процессе"));
    });

    connect(loader, &FileLoader::error,     [&, button](QString text){
        QMessageBox::information(this,"Danfoss Graph Generator",text,QMessageBox::Ok);
        button->setText(tr("Ошибка"));
        QTimer::singleShot(2000, this, [&](){
            button->setText(tr("Открыть"));
            button->blockSignals(false);
        });
    });
    connect(loader,&FileLoader::progress,   [&, button](int percent){
        button->setText(QString::number(percent) + "%");
    });
    connect(loader,&FileLoader::tableData,  [&](int type, QStringList data){
        switch (type) {
        case 0:
            titles = data;
            break;
        case 1:
            datas.append(data);
            break;
        default:
            break;
        }
    });
//    connect(loader,&FileLoader::readComplited, ui->selectGraph, &QWidget::show);
    connect(loader,&FileLoader::readComplited, [&](){
        graphWidget->setCalculationsData(CalculationsDatas);
        graphWidget->setOrdersData(OrdersDatas);
        graphWidget->makeWidgets();

        pptxWidget->setCalculationsData(CalculationsDatas);
        pptxWidget->setOrdersData(OrdersDatas);
        pptxWidget->makeWidgets();
    });
    connect(loader,&FileLoader::readComplited,  [&, button](){
        button->setText(tr("Готово"));
        QTimer::singleShot(2000, this, [&](){
            button->setText(tr("Открыть"));
            button->blockSignals(false);
        });
    });
    loader->start();
}

void MainWindow::fillData(int type, QStringList data)
{
    switch (type) {
    case 0:
        dataTitles = data;
        break;
    case 1:
        dataValues.append(data);
        break;
    default:
        break;
    }
}

//Graph * MainWindow::generateTwoBarsData(QStringList xDataList, QStringList yDataList)
//{
//    QMap<QVariant, QPair<int, int>> yDataValues;

//    QVariant minLimit, maxLimit;
//    switch (timeRangeIndex) {
//        case Weeks:{
//            minLimit = ui->weekFrom->currentText().remove(QRegExp("\\D")).toInt();
//            maxLimit = ui->weekTo->currentText().remove(QRegExp("\\D")).toInt();
//            break;
//        }
//        case Month:{
//              minLimit = monthList.indexOf(ui->monthFrom->currentText());
//              maxLimit = monthList.indexOf(ui->monthTo->currentText());
//            break;
//        }
//    }

//    int index = -1;
//    for (QString xData : xDataList){

//        index++;

//        QVariant data;
//        QVariant criterion;

//        QString string = dataValues.at(timeRangeFilter).at(index);
//        switch (timeRangeIndex) {
//            case Weeks:{
//                int week = string.remove(QRegExp("\\D")).toInt();
//                criterion = week;
//                break;
//            }
//            case Month:{
//                QDate date = QDate::fromString(string.remove(QRegExp("\\T.*")),"yyyy-M-d");
//                date.setDate(date.year(),date.month(),1);
//                criterion = date.month();
//                break;
//            }
//        }
//        if (divisionIndex >= 0){
//            QString dataString = xData;
//            dataString = dataString.remove(QRegExp("\\ \\-[\\w\\W\\d\\D]+$"));
//            dataString = dataString.simplified();
//            if (divisionIndex != 0 && divisionsList.at(divisionIndex).indexOf(dataString) == -1)
//                    continue;
//            data = xData;
//        } else {
//            data = criterion;
//        }

//        if (minLimit <= criterion && criterion <= maxLimit){
//            if (ui->cbGraphType->currentIndex() != 7){
//                int value = QString(yDataList.at(index)).remove(QRegExp("\\D")).toInt();
//                int filterValue = QString(dataValues.at(18).at(index)).remove(QRegExp("\\D")).toInt();
//                // Немного о магическом числе 3
//                // Если значение в ячейке превышает МЧ, то это число является "просроченным", иначе - нет.
//                if (filterValue > 3){
//                    yDataValues[data].first += value;
//                } else {
//                    yDataValues[data].second += value;
//                }
//            } else {
//                int value;
//                switch (ui->cbValueType->currentIndex()) {
//                case 0:
//                    value = 1;
//                    break;
//                case 1:
//                    value = QString(yDataList.at(index)).remove(QRegExp("\\D")).toInt();
//                    break;
//                }
//                QString filterValue = QString(dataValues.at(19).at(index));
//                if (filterValue == "нет"){
//                    yDataValues[data].first += value;
//                } else if (filterValue == "да"){
//                    yDataValues[data].second += value;
//                }
//            }
//        }
//    }

//    QVector<double> xDataValues, yDataUp, yDataDown;
//    QVector<QString> xDataLabels;

//    int counter = 1;
//    foreach (QVariant key, yDataValues.keys()){
//        if (divisionIndex < 0){
//                switch (timeRangeIndex) {
//                case Weeks:
//                    xDataLabels.append("W" + QString::number(key.toInt()));
//                    break;
//                case Month:
//                    QString date = monthList.at(key.toInt());
//                    xDataLabels.append(date);
//                    break;
//                }
//        } else {
//                xDataLabels.append(key.toString());
//        }
//        xDataValues.append(counter);
//        counter++;
//        yDataUp.append(yDataValues[key].first);
//        yDataDown.append(yDataValues[key].second);
//    }

//    Graph *barGraph = new Graph();
//    barGraph->setGraphTitle(graphTitle);
//    if (ui->cbGraphType->currentIndex() != 7){
//        barGraph->setAxisData(xDataValues,yDataUp,yDataDown,tr("просроченные задачи"), tr("непросроченные задачи"));
//    } else {
//        barGraph->setAxisData(xDataValues,yDataUp,yDataDown,tr("нет"), tr("да"), true);
//    }

//    barGraph->setAxisName(xTitle,yTitle);
//    barGraph->setAxisLabels(xDataLabels);

//    if (divisionIndex < 0){
//        barGraph->showLegend(true);
//        barGraph->showBarText(xDataValues,yDataUp,yDataDown, true);
//    } else {
//        barGraph->showBarText(xDataValues,yDataUp,yDataDown, false);
//    }
//    if (divisionIndex >=0 || timeRangeIndex == Month)
//        barGraph->rotateTicksNames(-45);

//    barGraph->rescale();
//    return barGraph;
//}

//Graph * MainWindow::generateHorizontalBarsData(QStringList xDataList, QStringList yDataList)
//{

//    QMap<QString, int>      yDataValues;

//    QVariant minLimit, maxLimit;
//    minLimit = ui->weekFrom->currentText().remove(QRegExp("\\D")).toInt();
//    maxLimit = ui->weekTo->currentText().remove(QRegExp("\\D")).toInt();

//    int index = -1;
//    for (QString xData : xDataList){
//        index++;
//        if (xData.isEmpty())
//            continue;
//        int criterion = QString(dataValues.at(timeRangeFilter).at(index)).remove(QRegExp("\\D")).toInt();;
//        if (!(minLimit <= criterion && maxLimit >= criterion))
//            continue;

//        int value = QString(yDataList.at(index)).remove(QRegExp("\\D")).toInt();
//        yDataValues[xData] += value;
//    }

//    QList<QPair<QString, int>> _yDataValues;
//    foreach (QString reason, reasonList) {
//        _yDataValues.append(QPair<QString, int>(reason, yDataValues[reason]));
//    }

//    for(int i = 0; i < _yDataValues.size(); i++){
//        for(int j = 0; j < _yDataValues.size() - 1 - i; j++){
//            if(_yDataValues.at(j).second < _yDataValues.at(j + 1).second){
//                QPair<QString, int> tmp = _yDataValues.at(j);
//                _yDataValues.replace(j, _yDataValues.at(j + 1));
//                _yDataValues.replace(j + 1, tmp);
//            }
//        }
//    }

//    QVector<double> xData, yData;
//    QVector<QString> xDataLabels;

//    int counter = 0;
//    for (QPair<QString, int> pair : _yDataValues){
//        yData.append(pair.second);
//        xData.append(++counter);
//        xDataLabels.append(pair.first);
//    }

//    Graph *barGraph = new Graph();
//    barGraph->setGraphTitle(graphTitle);
//    barGraph->setOrientation(Graph::Horizontal);
//    barGraph->setAxisData(xData,yData);
//    barGraph->setAxisLabels(xDataLabels);
//    barGraph->showBarText(xData,yData);
//    barGraph->showLegend(false);

//    barGraph->rescale();
//    return barGraph;
//}

void MainWindow::saveGraph()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Сохранить файл"),
                                QDir::currentPath()+"/"+graphTitle+".png",
                                "PNG (*.png)");
    if (fileName.isEmpty())
        return;

    QPushButton *pb = qobject_cast<QPushButton*>(QObject::sender());
    Graph *graph = pb->parent()->findChild<Graph *>();
    graph->savePng(fileName,graph->width(),graph->height(),1.0,100000);
}

void MainWindow::getMessage(QString text)
{
    QMessageBox::information(this,"Danfoss Graph Generator",text,QMessageBox::Ok);
}

//DTableWidgetItem::DTableWidgetItem(const QString &text, int type) :
//    QTableWidgetItem(text,type)
//{
//}

//bool DTableWidgetItem::operator <(const QTableWidgetItem&  other) const
//{
//    return(QDate::fromString(other.data( Qt::DisplayRole ).toString(), "d.M.yyyy") > QDate::fromString(data(Qt::DisplayRole ).toString(), "d.M.yyyy"));
//}

//WTableWidgetItem::WTableWidgetItem(const QString &text, int type) :
//    QTableWidgetItem(text,type)
//{
//}

//bool WTableWidgetItem::operator <(const QTableWidgetItem&  other) const
//{
//    return(other.data( Qt::DisplayRole ).toString().remove(QRegExp("\\D")).toDouble() > data( Qt::DisplayRole ).toString().remove(QRegExp("\\D")).toDouble());
//}

void MainWindow::setLocale(QString locale)
{
    if(translator->load(locale)){
        qDebug() << "Translator loadet successful \n";
        qApp->installTranslator(translator);
        ui->retranslateUi(this);

//        reloadLists();
//        reloadWidgets();

        return;
    }
    qDebug()<< "Could not load translation";
}
