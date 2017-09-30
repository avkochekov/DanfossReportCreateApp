#include "graph.h"
#include <algorithm>
#include <QMap>
#include <QVector>
#include <QVariant>

#define BAR_WIDTH   50
#define C_GREEN     QColor(10 , 200, 0)
#define C_RED       QColor(255, 0  , 0)

#define FONT_WEIGHT 11
//#define H_BLUE
//#define H_RED       QColor("#C00000");

Graph::Graph()
{
    this->plotLayout()->clear();
//    this->setGeometry(50,50,1000,150);
//    this->setWindowFlags(Qt::FramelessWindowHint);
    this->plotLayout()->addElement(0,0,title);
    this->plotLayout()->addElement(1,0,graphLayout);

    this->plotLayout()->setRowStretchFactor(0,0.001);
    this->plotLayout()->setRowStretchFactor(1,1);

    graphLayout->addElement(0,1, legendLayout);
    graphLayout->setColumnStretchFactor(0,1);
    graphLayout->setColumnStretchFactor(1,0.001);

    legendLayout->addElement(0, 0, dummyUpperElement);
    legendLayout->addElement(1, 0, legend);
    legendLayout->addElement(2, 0, dummyLowerElement);
    legendLayout->setRowStretchFactor(0,1);
    legendLayout->setRowStretchFactor(1,0.001);
    legendLayout->setRowStretchFactor(2,1);

    legend->setBorderPen(QPen(QColor(255,255,255)));
    legend->setSelectedBorderPen(QPen(QColor(255,255,255)));

    addNewGraph();
    setGraphOrientation();
}

void Graph::addNewGraph()
{
    legend->clear();
    legend->setFont(QFont(font().family(),FONT_WEIGHT));
//    legend->font().setPointSize(FONT_WEIGHT);

    graphRect = new QCPAxisRect(this, false);

    graphRect->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);

    graphLayout->addElement(0,0, graphRect);

    graphCount++;
}

void Graph::setGraphOrientation(Graph::Orientation orientation)
{
    graphOrientation = orientation;
    switch (graphOrientation) {
    case Horizontal:
        key = graphRect->axis(QCPAxis::atLeft);
        value = graphRect->axis(QCPAxis::atBottom);
        break;
    default:
        key = graphRect->axis(QCPAxis::atBottom);
        value = graphRect->axis(QCPAxis::atLeft);
        break;
    }
    key->grid()->setVisible(false);
    value->grid()->setVisible(true);

    //    value->ticker()->setTickCount(5);
    //    key->ticker()->setTickCount(5);

    value->setTickLabelFont(QFont(font().family(),FONT_WEIGHT));
    key->setTickLabelFont(QFont(font().family(),FONT_WEIGHT));

    value->setLabelFont(QFont(font().family(),FONT_WEIGHT));
    key->setLabelFont(QFont(font().family(),FONT_WEIGHT));

    value->setSubTicks(false);
    key->setSubTicks(false);

//    value->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
}

void Graph::setGraphTitle(QString name)
{
    title->setText(name);
    title->setFont(QFont("sans", 17, QFont::Bold));
}

void Graph::setData(QMap<QVariant, QPair<int, int> > data)
{
    upperBar = new QCPBars(this->key,this->value);
    lowerBar = new QCPBars(this->key,this->value);

    // Распологаем верхний upperBar над lowerBar
    upperBar->moveAbove(lowerBar);
    // Отключение антиалиасинга
    lowerBar->setAntialiased(false);
    upperBar->setAntialiased(false);

    upperBar->setWidth(0.8);
    lowerBar->setWidth(0.8);

    lowerBar->setPen(Qt::NoPen);
    upperBar->setPen(Qt::NoPen);

    int count = 1;
    foreach (QVariant key,data.keys()){
        keyLabel    .append(key.toString());
        keyValue    .append(count);
        lowerValue  .append(data[key].first);
        upperValue  .append(data[key].second);
        count++;
    }

    upperBar->setData(keyValue,upperValue);
    lowerBar->setData(keyValue,lowerValue);

//    if (!keyLabel.isEmpty()){
//        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
//        textTicker->addTicks(keyValue, keyLabel);
//        this->key->setTicker(textTicker);
//    }
//    if (!yDataName.isEmpty()){
//        QVector<double> ticks;
//        for (int count = 0; count < yDataName.size(); count++)
//            ticks.append(count + 1);
//        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
//        textTicker->addTicks(ticks, yDataName);
//        value->setTicker(textTicker);
//    }
}

void Graph::setDataLables(QString upper, QString lower)
{
    upperBar->setName(upper);
    lowerBar->setName(lower);
}

void Graph::setKeyAxisTickLables(QList<QString> labels, QList<double> values)
{

    QVector<QString>    labelsVector    = labels.toVector();
    QVector<double>     ticksVector     = values.toVector();
    if (!labelsVector.isEmpty() && labels.size() == ticksVector.size()){
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticksVector, labelsVector);

        key->setTicker(textTicker);
    }
}

void Graph::setAxisNames(QString keyAxis, QString valueAxis)
{
    key->setLabel(keyAxis);
    value->setLabel(valueAxis);
}

void Graph::setBarColors(QColor upper, QColor lower)
{
    upperBar->setBrush(upper);
    lowerBar->setBrush(lower);
}

void Graph::setBarColors(QColor color)
{
    upperBar->setBrush(color);
    lowerBar->setBrush(color);
}


void Graph::setAxisData(QVector<double> xData, QVector<double> yData, QString yDataName)
{
    QCPBars *bar = new QCPBars(key,value);
//    bar->setObjectName("horizontal");

    bar->setWidth(0.8);

    bar->setName(yDataName);

    bar->setData(xData, yData);
    bar->setAntialiased(false);
    bar->setPen(Qt::NoPen);
    bar->setBrush(C_GREEN.light(120));

    double maxValue = *std::max_element(yData.constBegin(),yData.constEnd());
    setAxisTickerStep(maxValue);

    graphWidth += xData.size() * BAR_WIDTH;
}

void Graph::setAxisData(QVector<double> xData, QVector<double> yDataUp, QVector<double> yDataDown, QString yDataNameUp, QString yDataNameDown, bool inversed)
{
    QCPBars *upperBar = new QCPBars(key,value);
//    upperBar->setObjectName("upper");
    QCPBars *lowerBar = new QCPBars(key,value);
//    upperBar->setObjectName("lower");

    upperBar->setWidth(0.5);
    lowerBar->setWidth(0.5);

    upperBar->setName(yDataNameUp);
    lowerBar->setName(yDataNameDown);

    upperBar->setData(xData, yDataUp);
    lowerBar->setData(xData, yDataDown);
    // распологаем верхний upperBar над lowerBar
    upperBar->moveAbove(lowerBar);
    // отключение антиалиасинга
    lowerBar->setAntialiased(false);
    upperBar->setAntialiased(false);
    // настройка цвета
    if (!inversed){
        lowerBar->setPen(Qt::NoPen);
        upperBar->setPen(Qt::NoPen);
        lowerBar->setBrush(C_GREEN.light(120));
        upperBar->setBrush(C_RED.light(120));
    } else {
        upperBar->setPen(Qt::NoPen);
        lowerBar->setPen(Qt::NoPen);
        upperBar->setBrush(C_GREEN.light(120));
        lowerBar->setBrush(C_RED.light(120));
    }

    double maxValue = *std::max_element(yDataUp.constBegin(),yDataUp.constEnd()) + *std::max_element(yDataDown.constBegin(),yDataDown.constEnd());
    setAxisTickerStep(maxValue);

    graphWidth += xData.size() * BAR_WIDTH;
}

void Graph::setAxisLabels(QVector<QString> xDataName, QVector<QString> yDataName)
{
    if (!xDataName.isEmpty()){
        QVector<double> ticks;
        for (int count = 0; count < xDataName.size(); count++)
            ticks.append(count + 1);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, xDataName);
        key->setTicker(textTicker);
    }
    if (!yDataName.isEmpty()){
        QVector<double> ticks;
        for (int count = 0; count < yDataName.size(); count++)
            ticks.append(count + 1);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, yDataName);
        value->setTicker(textTicker);
    }
}

void Graph::setBarColor(QColor color)
{
    foreach (QCPBars *bar, this->findChildren<QCPBars *>())
        bar->setBrush(color);
}

void Graph::showBarText(QVector<double> xData, QVector<double> yData)
{
    for (int index = 0; index < xData.size(); index++)
    {
        double xValue;
        double yValue;

        xValue = yData.at(index);
        yValue = xData.at(index);

//        switch (graphOrientation) {
//        case Horizontal:

//            break;
//        default:
//            xValue = xData.at(index);
//            yValue = yData.at(index);
//            break;
//        }

        QCPItemText *barText = new QCPItemText(this);
        barText->font().setPointSize(FONT_WEIGHT);
        barText->setFont(QFont(font().family(),FONT_WEIGHT));
        barText->setClipAxisRect(graphRect);
        switch (graphOrientation) {
        case Horizontal:
            barText->setText(QString::number(xValue));
            if (xValue > 0)
                barText->position->setCoords(xValue/2,yValue);
            else
                barText->position->setCoords(xValue,yValue);
            break;
        default:
            barText->setText(QString::number(yValue));
            barText->position->setCoords(xValue,yValue/2);
            break;
        }


        if (graphOrientation != Horizontal){
            QCPItemText *percentText = new QCPItemText(this);
            percentText->setFont(QFont(font().family(),FONT_WEIGHT));
            percentText->position->setCoords(xValue, yValue + yValue + 30);
            yValue = roundf((float)(yValue * 100) / (float)(yValue + yValue));
            percentText->setText(QString::number(yValue) + "%");
        }
    }
}

void Graph::showBarText(bool showPercents)
{

    for (int index = 0; index < keyValue.size(); index++)
    {
        int xValue      = keyValue.at(index);
        int yUpperValue = upperValue.at(index);
        int yLowerValue = lowerValue.at(index);

        int yValue = 0;

        if (yUpperValue != 0){
            QCPItemText *upperBarText = new QCPItemText(this);
            upperBarText->setFont(QFont(font().family(),FONT_WEIGHT));
            upperBarText->setClipAxisRect(graphRect);
            upperBarText->setText(QString::number(yUpperValue));
            switch (graphOrientation) {
            case Horizontal:
                upperBarText ->position->setCoords(yLowerValue + yUpperValue/2,xValue);
                break;
            default:
                upperBarText ->position->setCoords(xValue,yLowerValue + yUpperValue/2);
                break;
            }
        }

        if (yLowerValue != 0){
            QCPItemText *lowerBarText = new QCPItemText(this);
            lowerBarText->setFont(QFont(font().family(),FONT_WEIGHT));
            lowerBarText->setClipAxisRect(graphRect);
            lowerBarText->setText(QString::number(yLowerValue));
            switch (graphOrientation) {
            case Horizontal:
                lowerBarText->position->setCoords(yLowerValue/2,xValue);
                break;
            default:
                lowerBarText->position->setCoords(xValue,yLowerValue/2);
                break;
            }
        }

        if (showPercents){
            QCPItemText *percentText = new QCPItemText(this);
            percentText->setFont(QFont(font().family(),FONT_WEIGHT));
            percentText->setClipAxisRect(graphRect);
            percentText->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
            if (yUpperValue == 0 && yLowerValue == 0){
                yValue = 100;
            } else {
                yValue = (float)(yUpperValue * 100) / (float)(yUpperValue + yLowerValue);
            }
            percentText->setText(QString::number(yValue) + "%");
            switch (graphOrientation) {
            case Horizontal:
                percentText->position->setCoords((yUpperValue + yLowerValue) * 1.01, xValue);
                break;
            default:
                percentText->position->setCoords(xValue, (yUpperValue + yLowerValue) * 1.01);
                break;
            }
        }



    }
}

void Graph::rotateTicksNames(double xDegree, double yDegree)
{
    key->setTickLabelRotation(xDegree);
    value->setTickLabelRotation(yDegree);
}

void Graph::showLegend(bool show)
{
    legendLayout->setVisible(show);
}

void Graph::rescale()
{
    key->rescale();
    value->rescale();

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    this->rescaleAxes();
    switch (graphOrientation) {
    case Vertical:
        graphHeight = 500;
//        graphLayout->setMinimumSize(graphWidth, 600);
//        graphLayout->setMaximumSize(graphWidth, 600);
        key->setRange(key->range().lower - 0.5, key->range().upper + 0.5);
        graphWidth += BAR_WIDTH * 2;
        this->setGeometry(10,10,890,450);
        break;
    case Horizontal:
        graphHeight = 1000;
        graphWidth *= 0.4;
//        graphLayout->setMinimumSize(600, graphWidth);
//        graphLayout->setMaximumSize(600, graphWidth);
        key->setRange(key->range().lower - 0.5, key->range().upper + 0.5);
        graphWidth += BAR_WIDTH * 1;
        this->setGeometry(10,10,890,290);
        break;
    default:
        return;
        break;
    }
    value->setRangeUpper(value->range().upper * 1.1);
}

void Graph::setAxisName(QString xName, QString yName)
{
    key->setLabel(xName);
    value->setLabel(yName);
}

void Graph::saveToPng(QString fileName)
{
    this->savePng(fileName,0,0,1.0,1000);
}

void Graph::setAxisTickerStep(double maxValue)
{
    if (maxValue > 0){
        QSharedPointer<QCPAxisTickerFixed> valueTicker(new QCPAxisTickerFixed);
        double step = 10 * floor(log10(maxValue)) * 2;
        if (maxValue < step)
            step /= 10;
        valueTicker->setTickStep(step);
        value->setTicker(valueTicker);
        double upperRange = step * ceil(maxValue / step);
        value->setRangeUpper(upperRange * 1.1);
    } else {
        value->setRangeUpper(value->range().upper * 1.1);
    }
}

void Graph::configureAxis()
{
//    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
//    textTicker->addTicks(ticks, labels);
}
