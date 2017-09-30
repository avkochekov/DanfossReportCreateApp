#include "pptxcreator.h"
#include "ui_pptxcreator.h"

PptxCreator::PptxCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PptxCreator)
{
    ui->setupUi(this);
}

PptxCreator::~PptxCreator()
{
    delete ui;
}

void PptxCreator::on_pushButton_clicked()
{
    emit (ui->weekFrom->
}
