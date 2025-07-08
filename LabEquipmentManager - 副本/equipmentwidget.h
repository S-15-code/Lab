#ifndef EQUIPMENTWIDGET_H
#define EQUIPMENTWIDGET_H

#include <QWidget>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlError>  // 添加 QSqlError 头文件

class EquipmentWidget : public QWidget {
    Q_OBJECT

public:
    explicit EquipmentWidget(QWidget *parent = nullptr);
    void refresh();

private slots:
    void addEquipment();
    void editEquipment();
    void deleteEquipment();
    void searchEquipment();

private:
    QSqlTableModel *model;
    QTableView *view;
};

#endif // EQUIPMENTWIDGET_H
