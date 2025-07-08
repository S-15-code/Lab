#ifndef MAINTENANCEWIDGET_H
#define MAINTENANCEWIDGET_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlQuery>  // 添加 QSqlQuery 头文件
#include <QSqlError>  // 用于错误处理

class MaintenanceWidget : public QWidget {
    Q_OBJECT

public:
    explicit MaintenanceWidget(QWidget *parent = nullptr);
    void refresh();

private slots:
    void addMaintenance();
    void updateMaintenance();
    void completeMaintenance();
    void searchMaintenance();

private:
    QSqlRelationalTableModel *model;
    QTableView *view;
};

#endif // MAINTENANCEWIDGET_H
