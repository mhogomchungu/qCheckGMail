#-------------------------------------------------
#
# Project created by QtCreator 2013-06-06T06:50:08
#
#-------------------------------------------------

QT       += core gui

TARGET = qCheckGMail

TEMPLATE = app

OTHER_FILES += \
    README.md \
    qCheckGMailError.svg \
    qCheckGMail.svg \
    qCheckGMail.pro.user \
    qCheckGMail.notifyrc \
    qCheckGMail-NoMail.svg \
    qCheckGMail-GotMail.svg \
    CMakeLists.txt \
    cmake_uninstall.cmake.in \
    changelog \
    BUILD_INSTRUCTIONS

HEADERS += \
    walletmanager.h \
    qcheckgmail.h \
    configurationoptionsdialog.h \
    addaccount.h \
    accounts.h \
    statusicon.h \
    qcheckgmailplugin.h \
    task.h

SOURCES += \
    walletmanager.cpp \
    qcheckgmail.cpp \
    main.cpp \
    configurationoptionsdialog.cpp \
    addaccount.cpp \
    accounts.cpp \
    statusicon.cpp \
    qcheckgmailplugin.cpp \
    task.cpp

FORMS += \
    walletmanager.ui \
    configurationoptionsdialog.ui \
    addaccount.ui

RESOURCES += \
    icon.qrc

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/qCheckGMail-build/

LIBS += -lkdeui -lkwalletbackend -L/home/local/KDE4/lib -L/usr/lib -lgcrypt -llxqtwallet

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/spanish_ES.ts
TRANSLATIONS += translations.ts/french_FR.ts
