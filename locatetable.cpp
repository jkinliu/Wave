#include "locatetable.h"
#include "ui_locatetable.h"

LocateTable::LocateTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocateTable)
{
    ui->setupUi(this);
}

LocateTable::~LocateTable()
{
    delete ui;
}
