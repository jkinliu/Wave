#include "remotetable.h"
#include "ui_remotetable.h"

RemoteTable::RemoteTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteTable)
{
    ui->setupUi(this);
}

RemoteTable::~RemoteTable()
{
    delete ui;
}
