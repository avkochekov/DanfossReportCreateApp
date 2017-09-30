#ifndef INFOMESSAGE_H
#define INFOMESSAGE_H

#include <QWidget>

namespace Ui {
class InfoMessage;
}

class InfoMessage : public QWidget
{
    Q_OBJECT

public:
    explicit InfoMessage(QWidget *parent = 0);
    ~InfoMessage();

private:
    Ui::InfoMessage *ui;
};

#endif // INFOMESSAGE_H
