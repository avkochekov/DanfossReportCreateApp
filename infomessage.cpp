#include "infomessage.h"
#include "ui_infomessage.h"

InfoMessage::InfoMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoMessage)
{
    ui->setupUi(this);
}

InfoMessage::~InfoMessage()
{
    delete ui;
}
