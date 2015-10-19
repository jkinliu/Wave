#ifndef MODIFY_H
#define MODIFY_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include "global.h"

namespace Ui {
class Modify;
}

class Modify : public QDialog
{
    Q_OBJECT

public:
    explicit Modify(QWidget *parent = 0);
    ~Modify();

private slots:
    void setLineEmpty();
    void checkIpVaild();

private:
    Ui::Modify *ui;

    QDialogButtonBox *buttonBox;
    QLineEdit *name;
    QLineEdit *ip;
    QLineEdit *port;

public:
    void setName(QString nameStr);
    void setIp(QString ipStr);
    void setPort(QString portStr);
    QString getName();
    QString getIp();
    QString getPort();
};

#endif // MODIFY_H
