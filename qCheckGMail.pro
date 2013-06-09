#-------------------------------------------------
#
# Project created by QtCreator 2013-06-06T06:50:08
#
#-------------------------------------------------

QT       += core gui

TARGET = qCheckGMail

TEMPLATE = app

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/qCheckGMail-build/

LIBS += -lkdeui -lkwalletbackend -L/home/local/KDE4/lib -L/usr/lib

SOURCES += main.cpp \
    qcheckgmail.cpp \
    accounts.cpp \
    configurationdialog.cpp \
    addaccount.cpp \
    configurationoptionsdialog.cpp

HEADERS += \
    qcheckgmail.h \
    accounts.h \
    configurationdialog.h \
    addaccount.h \
    configurationoptionsdialog.h

FORMS += \
    configurationdialog.ui \
    addaccount.ui \
    configurationoptionsdialog.ui

TRANSLATIONS += translations.ts/translations.ts
