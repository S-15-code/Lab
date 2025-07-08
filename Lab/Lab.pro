#-------------------------------------------------
#
# Project created by QtCreator 2025-07-07T11:28:58
#
#-------------------------------------------------

QT       += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    databaseconnection.cpp \
    equipmentmanager.cpp \
    usermanager.cpp \
    borrowmanager.cpp \
    maintenancemanager.cpp

HEADERS  += mainwindow.h \
    databaseconnection.h \
    equipmentmanager.h \
    usermanager.h \
    borrowmanager.h \
    maintenancemanager.h

FORMS    += mainwindow.ui
