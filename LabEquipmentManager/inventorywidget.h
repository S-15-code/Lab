#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>  // 添加这行
#include <QSqlError>  // 建议同时添加
#include <QMessageBox>  // 添加这行

class InventoryWidget : public QWidget {
    Q_OBJECT

public:
    explicit InventoryWidget(QWidget *parent = nullptr);
    void refresh();

private slots:
    void checkInventory();

private:
    QSqlQueryModel *model;
    QTableView *view;
    QLabel *statusLabel;
};

#endif // INVENTORYWIDGET_H
