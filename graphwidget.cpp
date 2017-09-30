#include "graphwidget.h"
#include "ui_graphwidget.h"
#include <QDate>
#include <QStringList>
#include <datacalculator.h>
#include <graphwidget.h>
#include <graph.h>

#define GREEN       QColor(146, 208, 80 )
#define RED_LIGHT   QColor(255, 0  , 0  )
#define RED_DARK    QColor(192, 0  , 0  )
#define BLUE        QColor(79 , 129, 189)

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphWidget)
{
    ui->setupUi(this);

    reloadLists();
    ui->graphType->addItems(graphTypes);

    connect(ui->graphType,      SIGNAL(currentIndexChanged(int)), this, SLOT(setGraphType(int)));
    connect(ui->filterType,     SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterType(int)));
    connect(ui->divisionType,   SIGNAL(currentIndexChanged(int)), this, SLOT(setDivision(int)));

    connect(ui->dateFrom,   SIGNAL(currentIndexChanged(int)),this,SLOT(checkDates(int)));
    connect(ui->dateTo,     SIGNAL(currentIndexChanged(int)),this,SLOT(checkDates(int)));

    connect(this, &GraphWidget::graphTypeChanged, &GraphWidget::changeDataSources);
    connect(this, &GraphWidget::graphTypeChanged, &GraphWidget::changeWidgetsVisibility);

    connect(this, &GraphWidget::filterTypeChanged, &GraphWidget::makeDateFilters);

    connect(ui->selectAll,       &QPushButton::clicked, ui->reasonsList, &QListWidget::selectAll);
    connect(ui->removeSelection, &QPushButton::clicked, ui->reasonsList, &QListWidget::clearSelection);

    connect(ui->openGraph, &QPushButton::clicked, this, &GraphWidget::openGraph);

    connect(ui->valueCalculationType, SIGNAL(currentIndexChanged(int)), this, SLOT(setCountingMethod(int)));

    setGraphType(NONE);
}

GraphWidget::~GraphWidget()
{
    delete ui;
}

void GraphWidget::setCalculationsData(QList<QStringList> list)  {
    CalculationsData = list;
}

void GraphWidget::setOrdersData(QList<QStringList> list)
{
    OrdersData = list;
}

void GraphWidget::setLocale(QString locale)
{
//    if(translator->load(locale)){
//        qApp->installTranslator(translator);
//        ui->retranslateUi(this);

//        reloadLists();
//        reloadWidgets();
    //    }
}

void GraphWidget::makeWidgets()
{
    if (CurrentData.isEmpty())
        return;
    makeDateLists(CurrentData.at(DateColumn));

    if (!CalculationsData.isEmpty())
        makeReasonLists();

    makeDateFilters();
    makeReasonFilters();
}


void GraphWidget::reloadLists()
{
    graphTypes.clear();
    monthList.clear();
    divisionsList.clear();
    dataTitles.clear();


    graphTypes << tr("- Выберите номер графика -")
               << tr("1. Новые расчеты БТП")
               << tr("2. Корректировки расчетов БТП")
               << tr("3. Новые расчеты и корректировки БТП")
               << tr("4. Причины просроченных задач")
               << tr("5. Причины корректировок")
               << tr("6. Расчеты БТП по дивизионам")
               << tr("7. Заказы БТП");

    monthList << ""
              << tr("Январь")     << tr("Февраль")    << tr("Март")
              << tr("Апрель")     << tr("Май")        << tr("Июнь")
              << tr("Июль")       << tr("Август")     << tr("Сентябрь")
              << tr("Октябрь")    << tr("Ноябрь")     << tr("Декабрь");

    divisionsList.append(QStringList());
    divisionsList.append(QStringList() << tr("Краснодар - HE") << tr("Ростов-на-Дону - HE") << tr("Волгоград - HE") << tr("Саратов - HE"));
    divisionsList.append(QStringList() << tr("Владивосток - HE") << tr("Хабаровск - HE") << tr("Иркутск - HE") << tr("Красноярск - HE"));
    divisionsList.append(QStringList() << tr("Новосибирск - HE") << tr("Омск - HE"));
    divisionsList.append(QStringList() << tr("Санкт-Петербург - HE"));
    divisionsList.append(QStringList() << tr("Екатеринбург - HE") << tr("Ижевск - HE") << tr("Пермь - HE") << tr("Тюмень - HE") << tr("Челябинск - HE"));
    divisionsList.append(QStringList() << tr("Москва - HE") << tr("расчеты МСК ( Дудник, Карташева)"));
    divisionsList.append(QStringList() << tr("Казань - HE") << tr("Самара - HE") << tr("Уфа - HE") << tr("Н.Новгород - HE"));
    divisionsList.append(QStringList() << tr("Воронеж - HE") << tr("Ярославль - HE") << tr("Тула - HE"));

    dataTitles.append(tr("Количество расчетов, шт."));
    dataTitles.append(tr("Количество КП в задаче, шт."));
    dataTitles.append(tr("Офис продаж"));
}

void GraphWidget::reloadWidgets()
{
    reloadComboBox(ui->graphType, graphTypes);
    QList<int> dateList;
    switch (dateType) {
    case Month:
        dateList = availableMonths;
        break;
    case Week:
        dateList = availableWeeks;
        break;
    }
    reloadComboBox(ui->dateFrom, dateList);
    reloadComboBox(ui->dateTo  , dateList);
}

void GraphWidget::reloadComboBox(QComboBox *widget, QStringList list){
    int index = widget->currentIndex();
    if (index != -1){
        widget->blockSignals(true);
        widget->clear();
        widget->addItems(list);
        widget->blockSignals(false);
        widget->setCurrentIndex(index);
    }
}

void GraphWidget::reloadComboBox(QComboBox *widget, QList<int> list){
    int index = widget->currentIndex();
    if (index != -1){
        widget->blockSignals(true);
        widget->clear();
        QString itemText;
        foreach (int data, list) {
            switch (dateType) {
            case Month:
                itemText = monthList.at(data);
                break;
            case Week:
                itemText = QString::number(data);
                break;
            }
            widget->addItem(itemText,data);
        }
        widget->blockSignals(false);
        widget->setCurrentIndex(index);
    }
}

void GraphWidget::makeDateLists(QList<QString> dataIn){
    this->availableMonths.clear();
    this->availableWeeks .clear();
    foreach (QString string, dataIn) {
        if (string.isEmpty())
            continue;
        QDate date = QDate::fromString(string.remove(QRegExp("\\T.*")),"yyyy-M-d");
        availableMonths.append(date.month());
        availableWeeks .append(date.weekNumber());
    }
    availableMonths = availableMonths.toSet().toList();
    availableWeeks  = availableWeeks .toSet().toList();

    std::sort(availableMonths.begin(), availableMonths.end());
    std::sort(availableWeeks .begin(), availableWeeks .end());
}

void GraphWidget::makeDateFilters(){
    ui->dateFrom->clear();
    ui->dateTo->clear();

    QList <int> dateList;
    switch (dateType) {
    case Month:
        dateList = availableMonths;
        break;
    case Week:
        dateList = availableWeeks;
        break;
    }
    if (dateList.isEmpty())
        return;
    foreach (int date, dateList) {
        QString itemText;
        switch (dateType) {
        case Month:
            itemText = monthList.at(date);
            break;
        case Week:
            itemText = QString::number(date);
            break;
        }
        ui->dateFrom->addItem(itemText,date);
        ui->dateTo->addItem(itemText,date);
    }
    ui->dateFrom->setCurrentIndex(0);
    ui->dateTo  ->setCurrentIndex(dateList.size()-1);
}

void GraphWidget::makeReasonLists(){
    QStringList correctionReasonList    = CalculationsData.at(CorrectionReasonColumn);
    QStringList closingReasonList       = CalculationsData.at(ClosingReasonColumn);

    reasonsCorrection.clear();
    reasonsClosing.clear();

    pickReasonByDate(correctionReasonList,   reasonsCorrection);
    pickReasonByDate(closingReasonList,      reasonsClosing);

    makeReasonFilters();
}

void GraphWidget::pickReasonByDate(QStringList inList, QStringList &outList)
{
    for (int i = 0; i < inList.size(); i++){
        int date;
        QDate isoDate = QDate::fromString(CurrentData.at(DateColumn).at(i),Qt::ISODate);
        switch (dateType) {
        case Month:
            date = isoDate.month();
            break;
        case Week:
            date = isoDate.addDays(1).weekNumber();
            break;
        }
        if (dateFrom <= date && date <= dateTo){
            if (!inList.at(i).isEmpty())
                outList.append(inList.at(i));
        }
    }
    outList.removeDuplicates();
}

void GraphWidget::makeReasonFilters(){
    QStringList reasons;
    ui->reasonsList->clear();
    switch (reasonType) {
    case Correction:
        reasons = reasonsCorrection;
        break;
    case Closing:
        reasons = reasonsClosing;
        break;
    }

    ui->reasonsList->addItems(reasons);
    ui->reasonsList->sortItems();
}

void GraphWidget::setFilterType(int type)
{
    this->dateType = DateType(type);
    emit filterTypeChanged();
}

void GraphWidget::setCountingMethod(int index)
{
    countingMethod = Counting(index);
}

void GraphWidget::setDivision(int index)
{
    this->divisionIndex = index;
}

void GraphWidget::setGraphType(int type)
{
    graphType = GraphType(type);
    emit graphTypeChanged();
}

void GraphWidget::changeWidgetsVisibility()
{
    ui->filterType                  ->setEnabled(true);

    switch (graphType) {
    case NewCalculations:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(false);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(true);
        break;
    case CorrectionsOfCalculations:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(false);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(true);
        break;
    case NewCalculationsAndCorrections:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(false);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(true);
        break;
    case ClosingReasons:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(true);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(true);

        ui->filterType                  ->setEnabled(false);
        ui->filterType                  ->setCurrentIndex(Week);
        break;
    case CorrectionReasons:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(true);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(true);

        ui->filterType                  ->setEnabled(false);
        ui->filterType                  ->setCurrentIndex(Week);
        break;
    case ByDivisions:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(false);
        ui->divisionTypeWidget          ->setVisible(true);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(true);

        ui->filterType                  ->setEnabled(false);
        ui->filterType                  ->setCurrentIndex(Month);
        break;
    case Orders:
        ui->filtrationTypeWidget        ->setVisible(true);
        ui->reasonsWidget               ->setVisible(false);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(true);
        ui->openGraph                   ->setVisible(true);
        break;
    default:
        ui->filtrationTypeWidget        ->setVisible(false);
        ui->reasonsWidget               ->setVisible(false);
        ui->divisionTypeWidget          ->setVisible(false);
        ui->valueCalculationTypeWidget  ->setVisible(false);
        ui->openGraph                   ->setVisible(false);
        break;
    }
}

void GraphWidget::changeDataSources()
{
    DateColumn      = CalculationMonthColumn;
    countingMethod  = byValue;
    switch (graphType) {
    case NewCalculations:
        CurrentData = CalculationsData;

        xDataColumn = DateColumn;
        yDataColumn = NewCalculationsColumn;
        biasColumn  = TimeClosingColumn;
        timeColumn  = DateColumn;

        countingMethod = byValue;
        break;
    case CorrectionsOfCalculations:
        CurrentData = CalculationsData;

        xDataColumn = DateColumn;
        yDataColumn = CorrectionsColumn;
        biasColumn  = TimeClosingColumn;
        timeColumn  = DateColumn;

        countingMethod = byValue;
        break;
    case NewCalculationsAndCorrections:
        CurrentData = CalculationsData;

        xDataColumn = DateColumn;
        yDataColumn = CalcAndCorrectionsColumn;
        biasColumn  = TimeClosingColumn;
        timeColumn  = DateColumn;

        countingMethod = byValue;
        break;
    case ClosingReasons:
        CurrentData = CalculationsData;

        xDataColumn = ClosingReasonColumn;
        yDataColumn = CalcAndCorrectionsColumn;
        biasColumn  = 0;
        timeColumn  = DateColumn;

        reasonType  = Closing;

        countingMethod = byValue;
        break;
    case CorrectionReasons:
        CurrentData = CalculationsData;

        xDataColumn = CorrectionReasonColumn;
        yDataColumn = CorrectionsColumn;
        biasColumn  = 0;
        timeColumn  = DateColumn;

        reasonType  = Correction;

        countingMethod = byValue;
        break;
    case ByDivisions:
        CurrentData = CalculationsData;

        xDataColumn = AddressColumn;
        yDataColumn = CalcAndCorrectionsColumn;
        biasColumn  = TimeClosingColumn;
        timeColumn  = DateColumn;

        countingMethod = byValue;
        break;
    case Orders:
        DateColumn      = OrdersMonthColumn;

        CurrentData = OrdersData;

        xDataColumn = DateColumn;
        yDataColumn = NumberOfTasksColumn;
        biasColumn  = IsClosingColumn;
        timeColumn  = DateColumn;


        countingMethod = Counting(ui->valueCalculationType->currentIndex());
        break;
    default:
        CurrentData.clear();

        xDataColumn = 0;
        yDataColumn = 0;
        biasColumn  = 0;
        timeColumn  = 0;
        break;
    }

    makeWidgets();
}

void GraphWidget::checkDates(int index)
{
    dateFrom    = ui->dateFrom->currentData().toInt();
    dateTo      = ui->dateTo  ->currentData().toInt();
    QComboBox *cb = qobject_cast<QComboBox*>(QObject::sender());
    if (cb == ui->dateFrom){
        if (dateFrom > dateTo){
            dateFrom = dateTo;
            cb->setCurrentIndex(ui->dateTo->currentIndex());
        }
    }
    if (cb == ui->dateTo){
        if (dateTo < dateFrom){
            dateTo = dateFrom;
            cb->setCurrentIndex(ui->dateFrom->currentIndex());
        }
    }
    makeReasonLists();
}

void GraphWidget::openGraph()
{
    if (CurrentData.isEmpty())
        return;
    QStringList keyWords;
    switch (graphType) {
    case ByDivisions:
        keyWords = divisionsList.at(divisionIndex);
        break;
    case ClosingReasons:
    case CorrectionReasons:
        foreach (QListWidgetItem *item, ui->reasonsList->selectedItems()) {
            keyWords.append(item->text());
        }
        if (keyWords.isEmpty())
            return;
        break;
    default:
        keyWords.clear();
        break;
    }

    int         biasType    = DataCalculator::Number;
    int         dataType    = DataCalculator::Date;
    QVariant    biasValue   = 3;
    QString     graphTitle  = ui->graphType->currentText();

    graphTitle = graphTitle.remove(QRegExp("[0-9]+.\\s")).remove(QRegExp(" of BTP"));

    switch (graphType) {
    case Orders:
        biasType    = DataCalculator::Text;
        biasValue   = "да";
        break;
    case ByDivisions:
        graphTitle.clear();
    case ClosingReasons:
    case CorrectionReasons:
        dataType    = DataCalculator::Text;
        break;
    }

    // Рассчет данных для построения графиков
    DataCalculator *calculator = new DataCalculator();
    calculator->setDateData   (CurrentData.at(DateColumn));
    calculator->setDateType   (DataCalculator::DateType(dateType));
    calculator->setDateDataLimits (dateFrom,dateTo);
    calculator->setKeyWords   (keyWords);
    calculator->setBiasData   (CurrentData.at(biasColumn));
    calculator->setAxisData   (CurrentData.at(xDataColumn),CurrentData.at(yDataColumn));
    calculator->setAxisDataType   (DataCalculator::DataType(dataType));
    calculator->setBiasType   (DataCalculator::DataType(biasType));
    calculator->setBiasValue  (biasValue);
    calculator->setCountingMethod(DataCalculator::Counting(countingMethod));

    calculator->filterByDate();
    calculator->filterByKeyWords();
    calculator->calculate();

    QMap<QVariant, QPair<int, int>> data = calculator->getData();

    QList<QString> Labels;
    QList<double> Ticks;

    switch (graphType) {
    case ClosingReasons:
    case CorrectionReasons:
        {
            QList<QPair<QString, int>> dataList;
            foreach (QVariant key, data.keys()) {
                dataList.append(QPair<QString, int>(key.toString(), data[key].first));
            }

            for(int i = 0; i < dataList.size(); i++){
                for(int j = 0; j < dataList.size() - 1 - i; j++){
                    if(dataList.at(j).second < dataList.at(j + 1).second){
                        QPair<QString, int> tmp = dataList.at(j);
                        dataList.replace(j, dataList.at(j + 1));
                        dataList.replace(j + 1, tmp);
                    }
                }
            }

            data.clear();
            for (int i = 0; i < dataList.size(); i++){
                data[i].first   = dataList.at(i).second;
                Labels.append(dataList.at(i).first);
                Ticks.append(i);
            }
        }
        break;
    default:
        int counter = 1;
        foreach (QVariant key, data.keys()) {
            switch (graphType) {
            case ClosingReasons:
            case CorrectionReasons:
            case ByDivisions:
                Labels.append(key.toString());
                break;
            default:
                switch (dateType) {
                case Week:
                    Labels.append("W" + QString::number(key.toInt()));
                    break;
                case Month:
                    Labels.append(monthList.at(key.toInt()));
                    break;
                }
                break;
            }
            Ticks.append(counter);
            counter++;
        }
        break;
    }

    // Построение графиков
    Graph *graph = new Graph();
    graph->setGraphTitle(graphTitle);
    switch (graphType) {
    case ClosingReasons:
    case CorrectionReasons:
    case Orders:
        graph->setGraphOrientation(Graph::Horizontal);
        break;
    default:
        graph->setGraphOrientation(Graph::Vertical);
        break;
    }
    graph->setData(data);
    switch (graphType) {
    case Orders:
        graph->setBarColors(GREEN,RED_LIGHT);
        graph->setDataLables(tr("нет"), tr("да"));
        break;
    case ClosingReasons:
        graph->setBarColors(RED_DARK, RED_DARK);
        graph->showBarText(false);
        graph->showLegend(false);
        break;
    case CorrectionReasons:
        graph->setBarColors(BLUE,BLUE);
        graph->showBarText(false);
        graph->showLegend(false);
        break;
    default:
        graph->setBarColors(RED_LIGHT,GREEN);
        graph->showBarText();
        graph->setDataLables(tr("просроченные задачи"), tr("не просроченные задачи"));
        break;
    }
    graph->setKeyAxisTickLables(Labels,Ticks);
    graph->rescale();
    graph->show();
}

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

//void GraphWidget::setFilterType(int index)
//{
//    if (dataValues.isEmpty())
//        return;

//    timeRangeIndex = index;
//    ui->weeksRange->hide();
//    ui->monthRange->hide();
//    switch (index) {
//    case Weeks:
//        if (ui->cbGraphType->currentIndex() != 7){
//            timeRangeFilter = 17;
//        } else {
//            timeRangeFilter = 15;
//        }
//        ui->weeksRange->show();
//        xTitle = tr("Номер недели");
//        break;
//    case Month:
//        if (ui->cbGraphType->currentIndex() != 7){
//            timeRangeFilter = 13;
//        } else {
//            timeRangeFilter = 12;
//        }
//        ui->monthRange->show();
//        xTitle = tr("Месяц");
//        break;
//    default:
//        break;

//    generateFilters();
//    }
//}

//void GraphWidget::setGraphType(int index)
//{
//    ui->saveGraph->setEnabled(true);
//    dataValues = CalculationsDatas;
//    dataTitles = CalculationsDataTitles;
//    divisionIndex = -1;
//    ui->selectReasons->hide();
//    ui->divisionType->hide();

//    ui->ReasonAttic->hide();
//    ui->ReasonAdjust->hide();
//    ui->selectValues->hide();

//    ui->cbFilterType->setEnabled(true);

//    ui->filtrationType->setVisible(true);

//    graphTitle = ui->cbGraphType->currentText().remove(QRegExp("[0-9]+.\\s")).remove(QRegExp(" of BTP"));
//    switch (index) {
//    case 0:
//        ui->saveGraph->setEnabled(false);
//        break;
//    case 1:
//        KeyColumn = 17;
//        ValueColumn = 37;
////        ui->filtrationType->show();
//        yTitle = tr("Количество расчетов, шт");
//        break;
//    case 2:
//        KeyColumn = 17;
//        ValueColumn = 38;
////        ui->filtrationType->show();
//        yTitle = AxisTitles.at(0);
//        break;
//    case 3:
//        KeyColumn = 17;
//        ValueColumn = 39;
////        ui->filtrationType->show();
//        yTitle = AxisTitles.at(0);
//        break;
//    case 4:
//        KeyColumn = 20;
//        ValueColumn = 39;
//        ui->cbFilterType->setEnabled(false);
//        ui->cbFilterType->setCurrentIndex(Weeks);
////        ui->filtrationType->setVisible(true);
//        ui->selectReasons->show();
//        ui->ReasonAttic->show();
//        break;
//    case 5:
//        KeyColumn = 21;
//        ValueColumn = 38;
//        ui->cbFilterType->setEnabled(false);
//        ui->cbFilterType->setCurrentIndex(Weeks);
//        ui->selectReasons->show();
//        ui->ReasonAdjust->show();
//        break;
//    case 6:
//        KeyColumn = 0;
//        ValueColumn = 39;
//        ui->cbFilterType->setEnabled(false);
//        ui->cbFilterType->setCurrentIndex(Month);
//        ui->divisionType->show();
//        divisionIndex = ui->cbDivisionType->currentIndex();
//        graphTitle = "";
//        yTitle = AxisTitles.at(1);
//        xTitle = AxisTitles.at(2);
//        break;
//    case 7:
//        dataValues = OrdersDatas;
//        dataTitles = OrdersDataTitles;

//        WeeksColumn = 15;
//        MonthColumn = 12;

//        KeyColumn = 15;
//        ValueColumn = 17;
//        ui->cbFilterType->setCurrentIndex(Month);
////        xTitle = AxisTitles.at(2);

//        ui->selectValues->show();
//        break;
//    default:
//        ui->selectReasons->hide();
//        ui->divisionType->hide();
//        ui->filtrationType->hide();
//        break;
//    }
//    generateFilters();
//    generateReasonFilters();
//}

//void GraphWidget::setDivisionType(int index)
//{
//    divisionIndex = index;
//}

//void GraphWidget::on_selectAllReasonsAdjust_clicked()
//{
//    ui->listReasonsAdjust->selectAll();
//}

//void GraphWidget::on_removeSelectionAdjust_clicked()
//{
//    ui->listReasonsAdjust->clearSelection();
//}


//void GraphWidget::on_selectAllReasonsAttic_clicked()
//{
//    ui->listReasonsAttic->selectAll();
//}

//void GraphWidget::on_removeSelectionAttic_clicked()
//{
//    ui->listReasonsAttic->clearSelection();
//}

//void GraphWidget::setLocale(QString locale)
//{
//    if(translator->load(locale)){
//        qDebug() << "Translator loadet successful \n";
//        qApp->installTranslator(translator);
//        ui->retranslateUi(this);

//        reloadLists();
//        reloadWidgets();

//        return;
//    }
//    qDebug()<< "Could not load translation";
//}
