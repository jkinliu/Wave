#ifndef REMOTETABLE_H
#define REMOTETABLE_H

#include <QWidget>

namespace Ui {
class RemoteTable;
}

class RemoteTable : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteTable(QWidget *parent = 0);
    ~RemoteTable();

private:
    Ui::RemoteTable *ui;
};

#endif // REMOTETABLE_H
