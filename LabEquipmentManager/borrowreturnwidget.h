#ifndef BORROWRETURNWIDGET_H
#define BORRORRETURNWIDGET_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateEdit>
#include <QSqlQuery>  // 添加 QSqlQuery 头文件
#include <QSqlError>  // 添加 QSqlError 头文件

class BorrowReturnWidget : public QWidget {
    Q_OBJECT

public:
    explicit BorrowReturnWidget(QWidget *parent = nullptr);
    void refresh();

private slots:
    void borrowEquipment();
    void returnEquipment();
    void searchRecords();

private:
    QSqlRelationalTableModel *model;
    QTableView *view;
};

#endif // BORROWRETURNWIDGET_H
