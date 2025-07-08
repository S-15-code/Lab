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
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QDate>
class EquipmentWidget : public QWidget {
    Q_OBJECT

public:
    explicit EquipmentWidget(QWidget *parent = nullptr);
    void refresh();
    QSqlTableModel *model;


private slots:
    void addEquipment();
    void editEquipment();
    void deleteEquipment();
    void searchEquipment();

    // 新增状态管理槽函数
       void setEquipmentStatus(const QString& status);
       void markAsMaintenance();
       void markAsScrapped();
       void markAsBorrowed();
       void markAsReturned();


private:

    QTableView *view;
    // 新增私有方法
        bool changeStatus(int equipmentId, const QString& newStatus);

};

#endif // EQUIPMENTWIDGET_H
