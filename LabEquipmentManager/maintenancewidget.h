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
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class MaintenanceWidget;
}

class MaintenanceWidget : public QWidget {
    Q_OBJECT

public:
    explicit MaintenanceWidget(QWidget *parent = nullptr);
    ~MaintenanceWidget();
    void refresh();

private slots:
    void addMaintenance();
    void updateMaintenance();
    void completeMaintenance();
    void searchMaintenance();

private:
    QSqlTableModel *model;
    Ui::MaintenanceWidget *ui;
};

#endif // MAINTENANCEWIDGET_H
