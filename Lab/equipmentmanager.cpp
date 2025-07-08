#include "equipmentmanager.h"

EquipmentManager::EquipmentManager(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_db(db)
{
}

bool EquipmentManager::addEquipment(const Equipment &equipment)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Equipment (name, model, serial_number, purchase_date, "
                  "price, status, location, category, warranty_period, "
                  "last_maintenance_date, next_maintenance_date, threshold) "
                  "VALUES (:name, :model, :serial, :purchase, :price, :status, "
                  ":location, :category, :warranty, :last_maint, :next_maint, :threshold)");

    query.bindValue(":name", equipment.name);
    query.bindValue(":model", equipment.model);
    query.bindValue(":serial", equipment.serialNumber);
    query.bindValue(":purchase", equipment.purchaseDate);
    query.bindValue(":price", equipment.price);
    query.bindValue(":status", equipment.status);
    query.bindValue(":location", equipment.location);
    query.bindValue(":category", equipment.category);
    query.bindValue(":warranty", equipment.warrantyPeriod);
    query.bindValue(":last_maint", equipment.lastMaintenanceDate);
    query.bindValue(":next_maint", equipment.nextMaintenanceDate);
    query.bindValue(":threshold", equipment.threshold);

    if(!query.exec()) {
        qDebug() << "Error adding equipment:" << query.lastError().text();
        return false;
    }

    return true;
}

bool EquipmentManager::updateEquipment(const Equipment &equipment)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Equipment SET name=:name, model=:model, "
                  "serial_number=:serial, purchase_date=:purchase, "
                  "price=:price, status=:status, location=:location, "
                  "category=:category, warranty_period=:warranty, "
                  "last_maintenance_date=:last_maint, "
                  "next_maintenance_date=:next_maint, threshold=:threshold "
                  "WHERE id=:id");

    query.bindValue(":id", equipment.id);
    query.bindValue(":name", equipment.name);
    query.bindValue(":model", equipment.model);
    query.bindValue(":serial", equipment.serialNumber);
    query.bindValue(":purchase", equipment.purchaseDate);
    query.bindValue(":price", equipment.price);
    query.bindValue(":status", equipment.status);
    query.bindValue(":location", equipment.location);
    query.bindValue(":category", equipment.category);
    query.bindValue(":warranty", equipment.warrantyPeriod);
    query.bindValue(":last_maint", equipment.lastMaintenanceDate);
    query.bindValue(":next_maint", equipment.nextMaintenanceDate);
    query.bindValue(":threshold", equipment.threshold);

    if(!query.exec()) {
        qDebug() << "Error updating equipment:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool EquipmentManager::deleteEquipment(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Equipment WHERE id=:id");
    query.bindValue(":id", id);

    if(!query.exec()) {
        qDebug() << "Error deleting equipment:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<Equipment> EquipmentManager::getAllEquipment()
{
    QList<Equipment> equipmentList;
    QSqlQuery query(m_db);

    if(!query.exec("SELECT * FROM Equipment")) {
        qDebug() << "Error getting equipment:" << query.lastError().text();
        return equipmentList;
    }

    while(query.next()) {
        Equipment e;
        e.id = query.value("id").toInt();
        e.name = query.value("name").toString();
        e.model = query.value("model").toString();
        e.serialNumber = query.value("serial_number").toString();
        e.purchaseDate = query.value("purchase_date").toDate();
        e.price = query.value("price").toDouble();
        e.status = query.value("status").toInt();
        e.location = query.value("location").toString();
        e.category = query.value("category").toString();
        e.warrantyPeriod = query.value("warranty_period").toInt();
        e.lastMaintenanceDate = query.value("last_maintenance_date").toDate();
        e.nextMaintenanceDate = query.value("next_maintenance_date").toDate();
        e.threshold = query.value("threshold").toInt();

        equipmentList.append(e);
    }

    return equipmentList;
}

Equipment EquipmentManager::getEquipmentById(int id)
{
    Equipment e;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Equipment WHERE id=:id");
    query.bindValue(":id", id);

    if(!query.exec()) {
        qDebug() << "Error getting equipment by id:" << query.lastError().text();
        return e;
    }

    if(query.next()) {
        e.id = query.value("id").toInt();
        e.name = query.value("name").toString();
        e.model = query.value("model").toString();
        e.serialNumber = query.value("serial_number").toString();
        e.purchaseDate = query.value("purchase_date").toDate();
        e.price = query.value("price").toDouble();
        e.status = query.value("status").toInt();
        e.location = query.value("location").toString();
        e.category = query.value("category").toString();
        e.warrantyPeriod = query.value("warranty_period").toInt();
        e.lastMaintenanceDate = query.value("last_maintenance_date").toDate();
        e.nextMaintenanceDate = query.value("next_maintenance_date").toDate();
        e.threshold = query.value("threshold").toInt();
    }

    return e;
}

QList<Equipment> EquipmentManager::getEquipmentByStatus(int status)
{
    QList<Equipment> equipmentList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Equipment WHERE status=:status");
    query.bindValue(":status", status);

    if(!query.exec()) {
        qDebug() << "Error getting equipment by status:" << query.lastError().text();
        return equipmentList;
    }

    while(query.next()) {
        Equipment e;
        e.id = query.value("id").toInt();
        e.name = query.value("name").toString();
        e.model = query.value("model").toString();
        e.serialNumber = query.value("serial_number").toString();
        e.purchaseDate = query.value("purchase_date").toDate();
        e.price = query.value("price").toDouble();
        e.status = query.value("status").toInt();
        e.location = query.value("location").toString();
        e.category = query.value("category").toString();
        e.warrantyPeriod = query.value("warranty_period").toInt();
        e.lastMaintenanceDate = query.value("last_maintenance_date").toDate();
        e.nextMaintenanceDate = query.value("next_maintenance_date").toDate();
        e.threshold = query.value("threshold").toInt();

        equipmentList.append(e);
    }

    return equipmentList;
}

QList<Equipment> EquipmentManager::getEquipmentNeedMaintenance()
{
    QList<Equipment> equipmentList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Equipment WHERE next_maintenance_date <= :today OR status = 2");
    query.bindValue(":today", QDate::currentDate());

    if(!query.exec()) {
        qDebug() << "Error getting equipment needing maintenance:" << query.lastError().text();
        return equipmentList;
    }

    while(query.next()) {
        Equipment e;
        e.id = query.value("id").toInt();
        e.name = query.value("name").toString();
        e.model = query.value("model").toString();
        e.serialNumber = query.value("serial_number").toString();
        e.purchaseDate = query.value("purchase_date").toDate();
        e.price = query.value("price").toDouble();
        e.status = query.value("status").toInt();
        e.location = query.value("location").toString();
        e.category = query.value("category").toString();
        e.warrantyPeriod = query.value("warranty_period").toInt();
        e.lastMaintenanceDate = query.value("last_maintenance_date").toDate();
        e.nextMaintenanceDate = query.value("next_maintenance_date").toDate();
        e.threshold = query.value("threshold").toInt();

        equipmentList.append(e);
    }

    return equipmentList;
}

QList<Equipment> EquipmentManager::getEquipmentBelowThreshold()
{
    QList<Equipment> equipmentList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM Equipment WHERE threshold > 0 AND "
                  "(SELECT COUNT(*) FROM Equipment WHERE name = E.name) < E.threshold");

    if(!query.exec()) {
        qDebug() << "Error getting equipment below threshold:" << query.lastError().text();
        return equipmentList;
    }

    while(query.next()) {
        Equipment e;
        e.id = query.value("id").toInt();
        e.name = query.value("name").toString();
        e.model = query.value("model").toString();
        e.serialNumber = query.value("serial_number").toString();
        e.purchaseDate = query.value("purchase_date").toDate();
        e.price = query.value("price").toDouble();
        e.status = query.value("status").toInt();
        e.location = query.value("location").toString();
        e.category = query.value("category").toString();
        e.warrantyPeriod = query.value("warranty_period").toInt();
        e.lastMaintenanceDate = query.value("last_maintenance_date").toDate();
        e.nextMaintenanceDate = query.value("next_maintenance_date").toDate();
        e.threshold = query.value("threshold").toInt();

        equipmentList.append(e);
    }

    return equipmentList;
}
