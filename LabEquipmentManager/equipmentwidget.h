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
#include <QSqlError>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QDate>

namespace Ui {
class EquipmentWidget;
}

class EquipmentWidget : public QWidget {
    Q_OBJECT

public:
    explicit EquipmentWidget(QWidget *parent = nullptr);
    ~EquipmentWidget();
    void refresh();
    QSqlTableModel *model;
    void addMultipleEquipment(const QString& name, const QString& deviceModel, int quantity);
    QString generateSerialNumber(const QString& name, const QString& deviceModel, int index);

private slots:
    void addEquipment();
    void editEquipment();
    void deleteEquipment();
    void searchEquipment();
    void showScrapInfo();

private:
    // 新增私有方法
    bool changeStatus(int equipmentId, const QString& newStatus);
    Ui::EquipmentWidget *ui;
};

#endif // EQUIPMENTWIDGET_H
