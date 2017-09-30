#include "pptxwidget.h"
#include <graph.h>
#include "ui_pptxwidget.h"

#define GREEN       QColor(146, 208, 80 )
#define RED_LIGHT   QColor(255, 0  , 0  )
#define RED_DARK    QColor(192, 0  , 0  )
#define BLUE        QColor(79 , 129, 189)

PptxWidget::PptxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PptxWidget)
{
    ui->setupUi(this);
    connect(ui->createPresentation, &QPushButton::clicked, this, &PptxWidget::createPptx);

    graphTypes << tr("Новые расчеты БТП")
               << tr("Корректировки расчетов БТП")
               << tr("Новые расчеты и корректировки БТП")
               << tr("Новые расчеты и корректировки БТП")
               << tr("Причины просроченных задач")
               << tr("Причины корректировок")
               << tr("Заказы БТП");

    monthList << ""
              << tr("Январь")     << tr("Февраль")    << tr("Март")
              << tr("Апрель")     << tr("Май")        << tr("Июнь")
              << tr("Июль")       << tr("Август")     << tr("Сентябрь")
              << tr("Октябрь")    << tr("Ноябрь")     << tr("Декабрь");
}

PptxWidget::~PptxWidget()
{
    delete ui;
}

void PptxWidget::setCalculationsData(QList<QStringList> list)  {
    CalculationsData = list;
}

void PptxWidget::setOrdersData(QList<QStringList> list)
{
    OrdersData = list;
}

void PptxWidget::makeWidgets()
{
    if (CalculationsData.isEmpty() || OrdersData.isEmpty())
        return;
    makeDateLists();
    makeDateFilters();
}

void PptxWidget::makeDateLists()
{
    this->availableWeeks .clear();

    foreach (QString string, CalculationsData.at(CalculationDateColumn)) {
        if (string.isEmpty())
            continue;
        QDate date = QDate::fromString(string.remove(QRegExp("\\T.*")),"yyyy-M-d");
        availableWeeks.append(date.weekNumber());
        availableMonths.append(date.month());
    }
    foreach (QString string, OrdersData.at(OrdersDateColumn)) {
        if (string.isEmpty())
            continue;
        QDate date = QDate::fromString(string.remove(QRegExp("\\T.*")),"yyyy-M-d");
        availableWeeks.append(date.weekNumber());
        availableMonths.append(date.month());
    }
    availableWeeks  = availableWeeks.toSet().toList();
    availableMonths = availableMonths.toSet().toList();

    std::sort(availableWeeks .begin(), availableWeeks .end());
    std::sort(availableMonths.begin(), availableMonths.end());
}

void PptxWidget::makeDateFilters()
{
    ui->dateFrom->clear();
    ui->dateTo->clear();

    QList <int> dateList;
    if (availableWeeks.isEmpty())
        return;
    foreach (int date, availableWeeks){
        QString itemText = QString::number(date);
        ui->dateFrom->addItem(itemText,date);
        ui->dateTo->addItem(itemText,date);
    }
    ui->dateFrom->setCurrentIndex(0);
    ui->dateTo  ->setCurrentIndex(availableWeeks.size()-1);
}

void PptxWidget::makeImage(int graphIndex)
{
    QList<QString> Labels;
    QList<double> Ticks;

    switch (graphIndex) {
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
            switch (graphIndex) {
            case ClosingReasons:
            case CorrectionReasons:
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

    Graph *graph = new Graph();
//    graph->setGraphTitle(graphTitle);
    switch (graphIndex) {
    case ClosingReasons:
    case CorrectionReasons:
        graph->setGraphOrientation(Graph::Horizontal);
        break;
    default:
        graph->setGraphOrientation(Graph::Vertical);
        break;
    }
    graph->setData(data);
    switch (graphIndex) {
    case Orders:
        graph->setBarColors(GREEN,RED_LIGHT);
        graph->showBarText();
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
    graph->setGeometry(0,0,888,369);
    graph->setBackground(Qt::transparent);
    QDir().mkdir(QDir::currentPath() + "/pptx/imgs/");
    graph->savePng(QDir::currentPath() + "/pptx/imgs/" + QString::number(graphIndex) + ".png",graph->width(),graph->height(),1,100,182);
}

void PptxWidget::calculate()
{
    // Рассчет данных для построения графиков
    DataCalculator *calculator = new DataCalculator();
    calculator->setDateData   (CurrentData.at(DateColumn));
    calculator->setDateType   (DataCalculator::DateType(dateType));
    calculator->setDateDataLimits (dateFrom,dateTo);
    calculator->setBiasData   (CurrentData.at(biasColumn));
    calculator->setAxisData   (CurrentData.at(xDataColumn),CurrentData.at(yDataColumn));
    calculator->setAxisDataType   (DataCalculator::DataType(dataType));
    calculator->setBiasType   (DataCalculator::DataType(biasType));
    calculator->setBiasValue  (biasValue);
    calculator->setCountingMethod(DataCalculator::Counting(countingMethod));

    calculator->filterByDate();
    calculator->calculate();

    data = calculator->getData();
}

void PptxWidget::createPptx()
{
    if (OrdersData.isEmpty() || CalculationsData.isEmpty())
        return;
    QStringList argsList;
    int GraphsCount = 7;
    for (int graphIndex = 0; graphIndex < GraphsCount; graphIndex++){

        dateFrom        = ui->dateFrom->currentText().toInt();
        dateTo          = ui->dateTo  ->currentText().toInt();

        dateType        = Week;

        biasType        = DataCalculator::Number;
        dataType        = DataCalculator::Date;
        biasValue       = 3;
        countingMethod  = byValue;

        graphTitle      = graphTypes.at(graphIndex);

        DateColumn      = CalculationDateColumn;

        switch (graphIndex) {
        case NewCalculations:
            CurrentData = CalculationsData;

            xDataColumn = CalculationDateColumn;
            yDataColumn = NewCalculationsColumn;
            biasColumn  = TimeClosingColumn;
            xDataColumn = CalculationDateColumn;
            break;
        case CorrectionsOfCalculations:
            CurrentData = CalculationsData;

            xDataColumn = CalculationDateColumn;
            yDataColumn = CorrectionsColumn;
            biasColumn  = TimeClosingColumn;
            xDataColumn = CalculationDateColumn;
            break;
        case NewCalculationsAndCorrectionsByWeeks:
            CurrentData = CalculationsData;

            xDataColumn = CalculationDateColumn;
            yDataColumn = CalcAndCorrectionsColumn;
            biasColumn  = TimeClosingColumn;
            xDataColumn = CalculationDateColumn;

            dateType    = Week;
            break;
        case NewCalculationsAndCorrectionsByMonths:
            CurrentData = CalculationsData;

            xDataColumn = CalculationDateColumn;
            yDataColumn = CalcAndCorrectionsColumn;
            biasColumn  = TimeClosingColumn;
            xDataColumn = CalculationDateColumn;

            dateType    = Month;
            dateFrom    = availableMonths.first();
            dateTo      = availableMonths.last();

            break;
        case ClosingReasons:
            CurrentData = CalculationsData;

            xDataColumn = ClosingReasonColumn;
            yDataColumn = CalcAndCorrectionsColumn;
            biasColumn  = 0;
            xDataColumn = CalculationDateColumn;

            dateFrom    = dateTo - 2;

            reasonType  = Closing;

            dataType    = DataCalculator::Text;
            break;
        case CorrectionReasons:
            CurrentData = CalculationsData;

            xDataColumn = CorrectionReasonColumn;
            yDataColumn = CorrectionsColumn;
            biasColumn  = 0;
            xDataColumn = CalculationDateColumn;

            dateFrom    = dateTo - 2;

            reasonType  = Correction;

            dataType    = DataCalculator::Text;
            break;
        case Orders:
            CurrentData = OrdersData;

            xDataColumn = OrdersDateColumn;
            yDataColumn = NumberOfTasksColumn;
            biasColumn  = IsClosingColumn;
            xDataColumn = OrdersDateColumn;

            DateColumn  = OrdersDateColumn;

            biasType     = DataCalculator::Text;
            biasValue    = "да";
            break;
        default:
            return;
            break;
        }
        graphTitle = graphTitle.remove(QRegExp("[0-9]+.\\s")).remove(QRegExp(" of BTP"));
        calculate();
        makeImage(graphIndex);
        argsList.append(QDir::currentPath() + "\\pptx\\imgs\\" + QString::number(graphIndex) + ".png");
    }
    argsList.append("Presentation.pptx");
    if (QFile("pptx/spire.exe").exists()){
        qDebug() << "Pptx creator is found";
        QProcess *proc = new QProcess(this);
        connect(proc, &QProcess::started, [](){
            qDebug() << "Process started";
        });
        proc->setArguments(argsList);
        QString command = QDir::currentPath() + "/pptx/spire.exe";
        //command.replace("/", "\\");
        proc->startDetached(command);
    } else {
        qDebug() << "No pptx creator found";
    }
}
