#ifndef PPTXCREATOR_H
#define PPTXCREATOR_H

#include <QWidget>

namespace Ui {
class PptxCreator;
}

class PptxCreator : public QWidget
{
    Q_OBJECT

public:
    explicit PptxCreator(QWidget *parent = 0);
    ~PptxCreator();

private slots:
    void on_pushButton_clicked();

private:
    Ui::PptxCreator *ui;
};

#endif // PPTXCREATOR_H
