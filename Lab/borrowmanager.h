#ifndef BORROWMANAGER_H
#define BORROWMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QDate>
#include <QDebug>

struct BorrowRecord {
    int id;
    int equipmentId;
    int userId;
    QDate borrowDate;
    QDate expectedReturnDate;
    QDate actualReturnDate;
    QString borrowPurpose;
};

class BorrowManager : public QObject
{
    Q_OBJECT
public:
    explicit BorrowManager(QSqlDatabase db, QObject *parent = nullptr);

    bool borrowEquipment(int equipmentId, int userId, const QDate &borrowDate,
                        const QDate &expectedReturn, const QString &purpose);
    bool returnEquipment(int recordId, const QDate &returnDate);
    QList<BorrowRecord> getAllRecords();
    QList<BorrowRecord> getRecordsByEquipment(int equipmentId);
    QList<BorrowRecord> getRecordsByUser(int userId);
    QList<BorrowRecord> getOverdueRecords();

private:
    QSqlDatabase m_db;
};

#endif // BORROWMANAGER_H
