#include "modify.h"
#include "ui_modify.h"

Modify::Modify(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Modify)
{
    ui->setupUi(this);

    buttonBox = ui->buttonBox;
    name = ui->lineEdit_name;
    ip = ui->lineEdit_ip;
    port = ui->lineEdit_port;

    connect(buttonBox,SIGNAL(rejected()),this,SLOT(setLineEmpty()));
//    connect(buttonBox,SIGNAL(accepted()),this,SLOT(checkIpVaild()));
}

Modify::~Modify()
{
    delete ui;
}

void Modify::setLineEmpty()
{
    name->setText("");
    ip->setText("");
    port->setText("");
}

void Modify::checkIpVaild()
{
//    if(!ip->text().contains(QRegExp(IP_REG_EXP)))
//        ip->setText("");
}

void Modify::setName(QString nameStr)
{
    name->setText(nameStr);
}

void Modify::setIp(QString ipStr)
{
    ip->setText(ipStr);
}

void Modify::setPort(QString portStr)
{
    port->setText(portStr);
}

QString Modify::getName()
{
    return name->text();
}

QString Modify::getIp()
{
    return ip->text();
}

QString Modify::getPort()
{
    return port->text();
}


