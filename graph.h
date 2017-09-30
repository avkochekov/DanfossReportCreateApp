#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QWidget>
#include <QVector>
#include <qcustomplot.h>

class Graph : public QCustomPlot
{
public:
    enum Orientation{Vertical, Horizontal};

    Graph();
    void    addNewGraph();
    void    setData(QMap<QVariant, QPair<int, int> > data);
    void    setDataLables(QString upper, QString lower);
    void    setKeyAxisTickLables(QList<QString> labels, QList<double> ticks);
    void    setAxisNames(QString keyAxis = NULL, QString valueAxis = NULL);
    void    setBarColors(QColor upper, QColor lower);
    void    setBarColors(QColor color);
    void    setGraphOrientation(Orientation orientation = Vertical);
    void    setGraphTitle(QString name = NULL);
    void    showBarText(bool showPercents = true);

    void setAxisData(QVector<double> xData, QVector<double> yData, QString yDataName = NULL);
    void setAxisData(QVector<double> xData, QVector<double> yDataUp, QVector<double> yDataDown, QString yDataNameUp = NULL, QString yDataNameDown = NULL, bool inversed = false);

    void setAxisLabels(QVector<QString> xDataName = QVector<QString>(), QVector<QString> yDataName = QVector<QString>());

    void setBarColor(QColor color);

    void showBarText(QVector<double> xData, QVector<double> yData);

    void rotateTicksNames(double xDegree = 0, double yDegree = 0);
    void showLegend(bool show = false);
    void rescale();

    void setAxisName(QString xName = NULL, QString yName = NULL);
    void saveToPng(QString fileName = "DanfossGraph");

    void setAxisTickerStep(double maxValue);

private:
//    QCustomPlot *graph = new QCustomPlot();
    Orientation graphOrientation = Orientation::Vertical;

    // ////////////////////////////////////////
    QCPBars *upperBar;
    QCPBars *lowerBar;

    QVector<QString>     keyLabel;
    QVector<double>     keyValue;
    QVector<double>     upperValue;
    QVector<double>     lowerValue;

    QCPAxis *key;
    QCPAxis *value;
    // ////////////////////////////////////////
    int graphCount = 0;

    int graphWidth = 275;
    int graphHeight = 500;

    QMap<QString, QString>  xDataLabels;
    QMap<QString, double>   xDataValues;

    QVector<double> xValues;
    QVector<double> yUpperValues;
    QVector<double> yLowerValues;

    QMap<QString, double> yUpperDataValues;
    QMap<QString, double> yLowerDataValues;

//    QCPBars *upperBar = new QCPBars(this->xAxis,this->yAxis);
//    QCPBars *lowerBar = new QCPBars(this->xAxis,this->yAxis);

    QCPTextElement *title = new QCPTextElement(this);
    QCPLayoutGrid *graphLayout = new QCPLayoutGrid;

    QCPLegend *legend = new QCPLegend();

    QCPLayoutElement *dummyUpperElement = new QCPLayoutElement;
    QCPLayoutGrid *legendLayout = new QCPLayoutGrid;
    QCPLayoutElement *dummyLowerElement = new QCPLayoutElement;

//    QCPAxisRect *subRectTitle = new QCPAxisRect(this, false); // false means to not setup default axes
    QCPAxisRect *graphRect;


    void configureAxis();
};

#endif // GRAPH_H
