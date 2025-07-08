#include "maintenancemanager.h"

MaintenanceManager::MaintenanceManager(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_db(db)
{
}

bool MaintenanceManager::addMaintenanceRecord(int equipmentId, const QDate &date,
                                            const QString &type, const QString &desc,
                                            double cost, const QString &tech, int status)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Maintenance (equipment_id, maintenance_date, "
                  "maintenance_type, description, cost, technician, completion_status) "
                  "VALUES (:equip_id, :date, :type, :desc, :cost, :tech, :status)");

    query.bindValue(":equip_id", equipmentId);
    query.bindValue(":date", date);
    query.bindValue(":type", type);
    query.bindValue(":desc", desc);
    query.bindValue(":cost", cost);
    query.bindValue(":tech", tech);
    query.bindValue(":status", status);

    if(!query.exec()) {
        qDebug() << "Error adding maintenance record:" << query.lastError().text();
        return false;
    }

    return true;
}

bool MaintenanceManager::updateMaintenanceRecord(const MaintenanceRecord &record)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Maintenance SET equipment_id=:equip_id, maintenance_date=:date, "
                  "maintenance_type=:type, description=:desc, cost=:cost, "
                  "technician=:tech, completion_status=:status, "
                  "next_maintenance_date=:next_date WHERE id=:id");

    query.bindValue(":id", record.id);
    query.bindValue(":equip_id", record.equipmentId);
    query.bindValue(":date", record.maintenanceDate);
    query.bindValue(":type", record.maintenanceType);
    query.bindValue(":desc", record.description);
    query.bindValue(":cost", record.cost);
    query.bindValue(":tech", record.technician);
    query.bindValue(":status", record.completionStatus);
    query.bindValue(":next_date", record.nextMaintenanceDate);

    if(!query.exec()) {
        qDebug() << "Error updating maintenance record:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool MaintenanceManager::completeMaintenance(int recordId, const QDate &completionDate,
                                           const QDate &nextMaintenanceDate)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Maintenance SET completion_status=1, "
                  "next_maintenance_date=:next_date WHERE id=:id");
    query.bindValue(":id", recordId);
    query.bindValue(":next_date", nextMaintenanceDate);

    if(!query.exec()) {
        qDebug() << "Error completing maintenance:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<MaintenanceRecord> MaintenanceManager::getMaintenanceHistory(int equipmentId)
{
    QList<MaintenanceRecord> recordList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Maintenance WHERE equipment_id=:equip_id "
                  "ORDER BY maintenance_date DESC");
    query.bindValue(":equip_id", equipmentId);

    if(!query.exec()) {
        qDebug() << "Error getting maintenance history:" << query.lastError().text();
        return recordList;
    }

    while(query.next()) {
        MaintenanceRecord r;
        r.id = query.value("id").toInt();
        r.equipmentId = query.value("equipment_id").toInt();
        r.maintenanceDate = query.value("maintenance_date").toDate();
        r.maintenanceType = query.value("maintenance_type").toString();
        r.description = query.value("description").toString();
        r.cost = query.value("cost").toDouble();
        r.technician = query.value("technician").toString();
        r.completionStatus = query.value("completion_status").toInt();
        r.nextMaintenanceDate = query.value("next_maintenance_date").toDate();

        recordList.append(r);
    }

    return recordList;
}

QList<MaintenanceRecord> MaintenanceManager::getPendingMaintenance()
{
    QList<MaintenanceRecord> recordList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Maintenance WHERE completion_status=0");

    if(!query.exec()) {
        qDebug() << "Error getting pending maintenance:" << query.lastError().text();
        return recordList;
    }

    while(query.next()) {
        MaintenanceRecord r;
        r.id = query.value("id").toInt();
        r.equipmentId = query.value("equipment_id").toInt();
        r.maintenanceDate = query.value("maintenance_date").toDate();
        r.maintenanceType = query.value("maintenance_type").toString();
        r.description = query.value("description").toString();
        r.cost = query.value("cost").toDouble();
        r.technician = query.value("technician").toString();
        r.completionStatus = query.value("completion_status").toInt();
        r.nextMaintenanceDate = query.value("next_maintenance_date").toDate();

        recordList.append(r);
    }

    return recordList;
}
