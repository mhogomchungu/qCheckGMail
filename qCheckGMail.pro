#-------------------------------------------------
#
# Project created by QtCreator 2013-06-06T06:50:08
#
#-------------------------------------------------

QT       += core gui

TARGET = qCheckGMail

TEMPLATE = app

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/qCheckGMail-build/

LIBS += -lkdeui -lkwalletbackend -lphonon -L/home/local/KDE4/lib -L/usr/lib

SOURCES += main.cpp \
    qcheckgmail.cpp \
    accounts.cpp \
    kwalletmanager.cpp \
    addaccount.cpp \
    configurationoptionsdialog.cpp

HEADERS += \
    qcheckgmail.h \
    accounts.h \
    kwalletmanager.h \
    addaccount.h \
    configurationoptionsdialog.h

FORMS += \
    kwalletmanager.ui \
    addaccount.ui \
    configurationoptionsdialog.ui

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/spanish_ES.ts
TRANSLATIONS += translations.ts/french_FR.ts

