#-------------------------------------------------
#
# Project created by QtCreator 2013-06-06T06:50:08
#
#-------------------------------------------------

QT       += core gui

TARGET = qCheckGMail

TEMPLATE = app

INCLUDEPATH += /home/local/KDE4/include /usr/include

LIBS += -lkdeui -lkwalletbackend -L/home/local/KDE4/lib -L/usr/lib

SOURCES += main.cpp \
    qcheckgmail.cpp \
    accounts.cpp \
    configurationdialog.cpp

HEADERS += \
    qcheckgmail.h \
    accounts.h \
    configurationdialog.h

FORMS += \
    configurationdialog.ui
