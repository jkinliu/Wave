#ifndef LOCATETABLE_H
#define LOCATETABLE_H

#include <QWidget>

namespace Ui {
class LocateTable;
}

class LocateTable : public QWidget
{
    Q_OBJECT

public:
    explicit LocateTable(QWidget *parent = 0);
    ~LocateTable();

private:
    Ui::LocateTable *ui;
};

#endif // LOCATETABLE_H
