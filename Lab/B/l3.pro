#-------------------------------------------------
#
# Project created by QtCreator 2025-07-07T14:19:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = l3
TEMPLATE = app


SOURCES +=\
    ../13773739_3/LIMS/CSystemConfig.cpp \
    ../13773739_3/LIMS/DialogDevice.cpp \
    ../13773739_3/LIMS/DialogFault.cpp \
    ../13773739_3/LIMS/DialogLogin.cpp \
    ../13773739_3/LIMS/DialogServer.cpp \
    ../13773739_3/LIMS/DialogUser.cpp \
    ../13773739_3/LIMS/main.cpp \
    ../13773739_3/LIMS/mainwindow.cpp \
    ../13773739_3/LIMS/QMsgBox.cpp \
    ../13773739_3/LIMS/StringOperation.cpp \
    ../13773739_3/LIMS/WidgetDevice.cpp \
    ../13773739_3/LIMS/WidgetFault.cpp \
    ../13773739_3/LIMS/WidgetServer.cpp \
    ../13773739_3/LIMS/WidgetUser.cpp\
    ../13773739_3/LIMS/DatabaseHelper.cpp

HEADERS  += \
    ../13773739_3/LIMS/CSystemConfig.h \
    ../13773739_3/LIMS/DialogDevice.h \
    ../13773739_3/LIMS/DialogFault.h \
    ../13773739_3/LIMS/DialogLogin.h \
    ../13773739_3/LIMS/DialogServer.h \
    ../13773739_3/LIMS/DialogUser.h \
    ../13773739_3/LIMS/LIMSHeader.h \
    ../13773739_3/LIMS/mainwindow.h \
    ../13773739_3/LIMS/QMsgBox.h \
    ../13773739_3/LIMS/StringOperation.h \
    ../13773739_3/LIMS/WidgetDevice.h \
    ../13773739_3/LIMS/WidgetFault.h \
    ../13773739_3/LIMS/WidgetServer.h \
    ../13773739_3/LIMS/WidgetUser.h \
    ../13773739_3/LIMS/include/DatabaseHelper.h \
    ../13773739_3/LIMS/include/DatabaseHelperGlobal.h

FORMS    += \
    ../13773739_3/LIMS/DialogDevice.ui \
    ../13773739_3/LIMS/DialogFault.ui \
    ../13773739_3/LIMS/DialogLogin.ui \
    ../13773739_3/LIMS/DialogServer.ui \
    ../13773739_3/LIMS/DialogUser.ui \
    ../13773739_3/LIMS/mainwindow.ui \
    ../13773739_3/LIMS/QMsgBox.ui \
    ../13773739_3/LIMS/WidgetDevice.ui \
    ../13773739_3/LIMS/WidgetFault.ui \
    ../13773739_3/LIMS/WidgetServer.ui \
    ../13773739_3/LIMS/WidgetUser.ui

DISTFILES += \
    ../13773739_3/LIMS/LIMS.sdf \
    ../13773739_3/LIMS/qt_zh_CN.qm \
    ../13773739_3/LIMS/LIMS.v12.suo \
    ../13773739_3/LIMS/lims \
    ../13773739_3/LIMS/LIMS.pro.user \
    ../13773739_3/LIMS/LIMS.pro.user.795c320 \
    ../13773739_3/LIMS/LIMS.sln \
    ../13773739_3/LIMS/LIMS.vcxproj \
    ../13773739_3/LIMS/LIMS.vcxproj.filters \
    ../13773739_3/LIMS/LIMS.vcxproj.user

SUBDIRS += \
    ../13773739_3/LIMS/LIMS.pro

RESOURCES += \
    ../13773739_3/LIMS/image.qrc \
    ../13773739_3/LIMS/lims.qrc

QT += sql
