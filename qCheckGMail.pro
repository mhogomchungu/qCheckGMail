#-------------------------------------------------
#
# Project created by QtCreator 2013-06-06T06:50:08
#
#-------------------------------------------------

QT       += core gui

TARGET = qCheckGMail

TEMPLATE = app

SUBDIRS = . lxqt_wallet

OTHER_FILES += \
    README.md \
    icons/qCheckGMailError.svg \
    icons/qCheckGMail.svg \
    icons/qCheckGMail.pro.user \
    icons/qCheckGMail.notifyrc \
    icons/qCheckGMail-NoMail.svg \
    icons/qCheckGMail-GotMail.svg \
    CMakeLists.txt \
    cmake_uninstall.cmake.in \
    changelog \
    BUILD_INSTRUCTIONS

HEADERS += \
    src/walletmanager.h \
    src/qcheckgmail.h \
    src/configurationoptionsdialog.h \
    src/addaccount.h \
    src/accounts.h \
    src/statusicon.h \
    src/qcheckgmailplugin.h \
    src/task.h

SOURCES += \
    src/walletmanager.cpp \
    src/qcheckgmail.cpp \
    src/main.cpp \
    src/configurationoptionsdialog.cpp \
    src/addaccount.cpp \
    src/accounts.cpp \
    src/statusicon.cpp \
    src/qcheckgmailplugin.cpp \
    src/task.cpp

FORMS += \
    src/walletmanager.ui \
    src/configurationoptionsdialog.ui \
    src/addaccount.ui

RESOURCES += \
    icons/icon.qrc

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/projects/build/build-qCheckGMail-Desktop-Debug

LIBS += -lkdeui -lkwalletbackend -L/home/local/KDE4/lib -L/usr/lib -lgcrypt -llxqtwallet

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/spanish_ES.ts
TRANSLATIONS += translations.ts/french_FR.ts
