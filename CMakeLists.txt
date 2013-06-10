
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

SET( QT_USE_QTNETWORK TRUE )

find_package( Qt4 COMPONENTS REQUIRED )

find_package( KDE4 REQUIRED )

include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

add_definitions( -I${KDE4_INCLUDE_DIR} )
add_definitions( -I${Qt4_INCLUDE_DIR} )
add_definitions( -I${QT_HEADERS_DIR} )
add_definitions( -I${PHONON_INCLUDE_DIR} )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

QT4_WRAP_CPP( MOC qcheckgmail.h configurationdialog.h addaccount.h configurationoptionsdialog.h )

QT4_WRAP_UI( UI configurationdialog.ui addaccount.ui configurationoptionsdialog.ui )

add_executable( qCheckGMail main.cpp qcheckgmail.cpp accounts.cpp 
	configurationdialog.cpp addaccount.cpp configurationoptionsdialog.cpp ${MOC} ${UI} ${ICONS} )

TARGET_LINK_LIBRARIES( qCheckGMail -L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} -lkdeui -lkwalletbackend -lQtNetwork -lphonon ) 

set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )

install ( FILES qCheckGMail-GotMail.svg DESTINATION share/icons )
install ( FILES qCheckGMail-NoMail.svg DESTINATION share/icons )
install ( FILES qCheckGMail.svg DESTINATION share/icons )
install ( FILES qCheckGMailError.svg DESTINATION share/icons )
install ( FILES qCheckGMailNewMail.ogg DESTINATION share/qCheckGMail )

install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

install ( SCRIPT "${CMAKE_SOURCE_DIR}/PostInstall.cmake" )

install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications 
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install ( FILES translations.ts/translations.ts DESTINATION share/qCheckGMail/translations.ts )

#install ( FILES qCheckGMail.notifyrc DESTINATION share/apps/qCheckGMail/ )

install ( SCRIPT "${CMAKE_SOURCE_DIR}/PostInstall.cmake" )

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

file( WRITE "${PROJECT_BINARY_DIR}/language_path.h"
"\n#define LANGUAGE_FILE_PATH \"${CMAKE_INSTALL_PREFIX}/share/qCheckGMail/translations.qm/\""
)

file( WRITE "${PROJECT_BINARY_DIR}/new_mail_audio_path.h"
"\n#define AUDIO_FILE_PATH \"${CMAKE_INSTALL_PREFIX}/share/qCheckGMail/qCheckGMailNewMail.ogg\""
)

# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/qCheckGMail.desktop"
"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/qCheckGMail
GenericName[en_US]=a gmail new mail checker
GenericName=a gmail new mail checker
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/qCheckGMail.svg
MimeType=
Name[en_US]=qCheckGMail
Name=qCheckGMail
NoDisplay=false
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=Unique
X-KDE-SubstituteUID=false
X-KDE-Username=
Categories=Utility;\n")

# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop"
"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/qCheckGMail -a
GenericName[en_US]=a gmail new mail checker
GenericName=a gmail new mail checker
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/qCheckGMail.svg
MimeType=
Name[en_US]=qCheckGMail
Name=qCheckGMail
NoDisplay=false
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=Unique
X-KDE-SubstituteUID=false
X-KDE-Username=
Categories=Utility;\n")

