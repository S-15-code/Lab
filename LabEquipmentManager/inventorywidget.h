#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>

namespace Ui {
class InventoryWidget;
}

class InventoryWidget : public QWidget {
    Q_OBJECT

public:
    explicit InventoryWidget(QWidget *parent = nullptr);
    ~InventoryWidget();
    void refresh();

private slots:
    void checkInventory();

private:
    QSqlQueryModel *model;
    Ui::InventoryWidget *ui;
};

#endif // INVENTORYWIDGET_H
