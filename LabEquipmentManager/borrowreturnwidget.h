#ifndef BORROWRETURNWIDGET_H
#define BORROWRETURNWIDGET_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateEdit>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class BorrowReturnWidget;
}

class BorrowReturnWidget : public QWidget {
    Q_OBJECT

public:
    explicit BorrowReturnWidget(QWidget *parent = nullptr);
    ~BorrowReturnWidget();
    void refresh();

private slots:
    void borrowEquipment();
    void returnEquipment();
    void searchRecords();

private:
    QSqlRelationalTableModel *model;
    Ui::BorrowReturnWidget *ui;
};

#endif // BORROWRETURNWIDGET_H
