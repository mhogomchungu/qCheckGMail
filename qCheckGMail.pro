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

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/spanish_ES.ts
TRANSLATIONS += translations.ts/french_FR.ts

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
    BUILD_INSTRUCTIONS \
    storage_backends/backend/README \
    storage_backends/backend/CMakeLists.txt \
    storage_backends/frontend/README \
    storage_backends/frontend/CMakeLists.txt

HEADERS += \
    walletmanager.h \
    qcheckgmail.h \
    configurationoptionsdialog.h \
    addaccount.h \
    accounts.h \
    storage_backends/backend/lxqtwallet.h \
    storage_backends/frontend/password_dialog.h \
    storage_backends/frontend/open_wallet_thread.h \
    storage_backends/frontend/lxqt_wallet_interface.h \
    storage_backends/frontend/lxqt_kwallet.h \
    storage_backends/frontend/lxqt_internal_wallet.h \
    storage_backends/frontend/changepassworddialog.h \
    statusicon.h \
    qcheckgmailplugin.h

SOURCES += \
    walletmanager.cpp \
    qcheckgmail.cpp \
    main.cpp \
    configurationoptionsdialog.cpp \
    addaccount.cpp \
    accounts.cpp \
    storage_backends/backend/lxqtwallet.c \
    storage_backends/frontend/password_dialog.cpp \
    storage_backends/frontend/open_wallet_thread.cpp \
    storage_backends/frontend/lxqt_wallet_interface.cpp \
    storage_backends/frontend/lxqt_kwallet.cpp \
    storage_backends/frontend/lxqt_internal_wallet.cpp \
    storage_backends/frontend/changepassworddialog.cpp \
    statusicon.cpp \
    qcheckgmailplugin.cpp

FORMS += \
    walletmanager.ui \
    configurationoptionsdialog.ui \
    addaccount.ui \
    storage_backends/frontend/password_dialog.ui \
    storage_backends/frontend/changepassworddialog.ui

RESOURCES += \
    icon.qrc
