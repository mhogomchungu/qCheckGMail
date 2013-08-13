#-------------------------------------------------
#
# Project created by QtCreator 2013-06-06T06:50:08
#
#-------------------------------------------------

QT       += core gui

TARGET = qCheckGMail

TEMPLATE = app

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/qCheckGMail-build/

LIBS += -lkdeui -lkwalletbackend -L/home/local/KDE4/lib -L/usr/lib -lgcrypt

SOURCES += main.cpp \
    qcheckgmail.cpp \
    accounts.cpp \
    walletmanager.cpp \
    addaccount.cpp \
    configurationoptionsdialog.cpp \
    storage_backends/lxqtwallet.c \
    storage_backends/password_dialog.cpp \
    storage_backends/lxqt_wallet_interface.cpp \
    storage_backends/lxqt_kwallet.cpp \
    storage_backends/lxqt_internal_wallet.cpp \
    storage_backends/open_wallet_thread.cpp

HEADERS += \
    qcheckgmail.h \
    accounts.h \
    walletmanager.h \
    addaccount.h \
    configurationoptionsdialog.h \
    storage_backends/password_dialog.h \
    storage_backends/lxqtwallet.h \
    storage_backends/lxqt_wallet_interface.h \
    storage_backends/lxqt_kwallet.h \
    storage_backends/lxqt_internal_wallet.h \
    storage_backends/open_wallet_thread.h

FORMS += \
    walletmanager.ui \
    addaccount.ui \
    configurationoptionsdialog.ui \
	storage_backends/password_dialog.ui

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/spanish_ES.ts
TRANSLATIONS += translations.ts/french_FR.ts

