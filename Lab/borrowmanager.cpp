#include "borrowmanager.h"

BorrowManager::BorrowManager(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_db(db)
{
}

bool BorrowManager::borrowEquipment(int equipmentId, int userId, const QDate &borrowDate,
                                   const QDate &expectedReturn, const QString &purpose)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO BorrowRecord (equipment_id, user_id, borrow_date, "
                  "expected_return_date, borrow_purpose) "
                  "VALUES (:equip_id, :user_id, :borrow_date, :expected_return, :purpose)");

    query.bindValue(":equip_id", equipmentId);
    query.bindValue(":user_id", userId);
    query.bindValue(":borrow_date", borrowDate);
    query.bindValue(":expected_return", expectedReturn);
    query.bindValue(":purpose", purpose);

    if(!query.exec()) {
        qDebug() << "Error adding borrow record:" << query.lastError().text();
        return false;
    }

    return true;
}

bool BorrowManager::returnEquipment(int recordId, const QDate &returnDate)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE BorrowRecord SET actual_return_date=:return_date WHERE id=:id");
    query.bindValue(":return_date", returnDate);
    query.bindValue(":id", recordId);

    if(!query.exec()) {
        qDebug() << "Error updating return record:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<BorrowRecord> BorrowManager::getAllRecords()
{
    QList<BorrowRecord> recordList;
    QSqlQuery query(m_db);

    if(!query.exec("SELECT * FROM BorrowRecord")) {
        qDebug() << "Error getting all borrow records:" << query.lastError().text();
        return recordList;
    }

    while(query.next()) {
        BorrowRecord r;
        r.id = query.value("id").toInt();
        r.equipmentId = query.value("equipment_id").toInt();
        r.userId = query.value("user_id").toInt();
        r.borrowDate = query.value("borrow_date").toDate();
        r.expectedReturnDate = query.value("expected_return_date").toDate();
        r.actualReturnDate = query.value("actual_return_date").toDate();
        r.borrowPurpose = query.value("borrow_purpose").toString();

        recordList.append(r);
    }

    return recordList;
}

QList<BorrowRecord> BorrowManager::getRecordsByEquipment(int equipmentId)
{
    QList<BorrowRecord> recordList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM BorrowRecord WHERE equipment_id=:equip_id");
    query.bindValue(":equip_id", equipmentId);

    if(!query.exec()) {
        qDebug() << "Error getting records by equipment:" << query.lastError().text();
        return recordList;
    }

    while(query.next()) {
        BorrowRecord r;
        r.id = query.value("id").toInt();
        r.equipmentId = query.value("equipment_id").toInt();
        r.userId = query.value("user_id").toInt();
        r.borrowDate = query.value("borrow_date").toDate();
        r.expectedReturnDate = query.value("expected_return_date").toDate();
        r.actualReturnDate = query.value("actual_return_date").toDate();
        r.borrowPurpose = query.value("borrow_purpose").toString();

        recordList.append(r);
    }

    return recordList;
}

QList<BorrowRecord> BorrowManager::getRecordsByUser(int userId)
{
    QList<BorrowRecord> recordList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM BorrowRecord WHERE user_id=:user_id");
    query.bindValue(":user_id", userId);

    if(!query.exec()) {
        qDebug() << "Error getting records by user:" << query.lastError().text();
        return recordList;
    }

    while(query.next()) {
        BorrowRecord r;
        r.id = query.value("id").toInt();
        r.equipmentId = query.value("equipment_id").toInt();
        r.userId = query.value("user_id").toInt();
        r.borrowDate = query.value("borrow_date").toDate();
        r.expectedReturnDate = query.value("expected_return_date").toDate();
        r.actualReturnDate = query.value("actual_return_date").toDate();
        r.borrowPurpose = query.value("borrow_purpose").toString();

        recordList.append(r);
    }

    return recordList;
}

QList<BorrowRecord> BorrowManager::getOverdueRecords()
{
    QList<BorrowRecord> recordList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM BorrowRecord WHERE actual_return_date IS NULL "
                  "AND expected_return_date < :today");
    query.bindValue(":today", QDate::currentDate());

    if(!query.exec()) {
        qDebug() << "Error getting overdue records:" << query.lastError().text();
        return recordList;
    }

    while(query.next()) {
        BorrowRecord r;
        r.id = query.value("id").toInt();
        r.equipmentId = query.value("equipment_id").toInt();
        r.userId = query.value("user_id").toInt();
        r.borrowDate = query.value("borrow_date").toDate();
        r.expectedReturnDate = query.value("expected_return_date").toDate();
        r.actualReturnDate = query.value("actual_return_date").toDate();
        r.borrowPurpose = query.value("borrow_purpose").toString();

        recordList.append(r);
    }

    return recordList;
}
