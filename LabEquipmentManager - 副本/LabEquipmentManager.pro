#-------------------------------------------------
#
# Project created by QtCreator 2025-07-08T09:54:58
#
#-------------------------------------------------

QT       += core gui

QT += core gui widgets sql  # 添加 sql 模块
TARGET = LabEquipmentManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    equipmentwidget.cpp \
    borrowreturnwidget.cpp \
    maintenancewidget.cpp \
    inventorywidget.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    equipmentwidget.h \
    borrowreturnwidget.h \
    maintenancewidget.h \
    inventorywidget.h
