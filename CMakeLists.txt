
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

find_package( Qt4 COMPONENTS REQUIRED )

find_package( KDE4 REQUIRED )

include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

add_definitions( -I${KDE4_INCLUDE_DIR} )
add_definitions( -I${Qt4_INCLUDE_DIR} )
add_definitions( -I${QT_HEADERS_DIR} )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

QT4_WRAP_CPP( MOC qcheckgmail.h kwalletmanager.h addaccount.h configurationoptionsdialog.h )

QT4_WRAP_UI( UI kwalletmanager.ui addaccount.ui configurationoptionsdialog.ui )

add_executable( qCheckGMail main.cpp qcheckgmail.cpp accounts.cpp 
		kwalletmanager.cpp addaccount.cpp configurationoptionsdialog.cpp ${MOC} ${UI} ${ICONS} )

find_library( LIBKWALLETBACKEND libkwalletbackend.so )
if( NOT LIBKWALLETBACKEND )
	# some distributions do not have libkwalletbackend.so for reasons i currently do not know
	# look for libkwalletbackend.sp.4 when the above happen
	find_library( LIBKWALLETBACKEND libkwalletbackend.so.4 )	
endif()

if( NOT LIBKWALLETBACKEND )
	message( FATAL_ERROR "could not find libkwalletbackend.so or libkwalletbackend.so.4,please make sure you have kwallet development libraries installed and try again" )
endif()

find_library( LIBKDEUI libkdeui.so )
if( NOT LIBKDEUI )
	# adding this one just in case the same case as above happen
	find_library( LIBKDEUI libkdeui.so.5 )
endif()
	
if( NOT LIBKDEUI )
	message( FATAL_ERROR "could not find libkdeui.so or libkdeui.so.5,please make sure you have kde development libraries installed and try again" )
endif()

find_library( LIBKDECORE libkdecore.so )
if( NOT LIBKDECORE )
	# adding this one just in case the same case as above happen	
	find_library( LIBKDECORE libkdecore.so.5 )
endif()

if( NOT LIBKDECORE )
	message( FATAL_ERROR "could not find libkdecore.so or libkdecore.so.5,please make sure you have kde development libraries installed and try again" )
endif()

#
# If linking failed because of missing library,add the library at the end of below line inside the brackets
#
TARGET_LINK_LIBRARIES( qCheckGMail -L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE} ${LIBKWALLETBACKEND} ) 

set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )

install ( FILES qCheckGMail-GotMail.svg DESTINATION share/icons )
install ( FILES qCheckGMail-NoMail.svg DESTINATION share/icons )
install ( FILES qCheckGMail.svg DESTINATION share/icons )
install ( FILES qCheckGMailError.svg DESTINATION share/icons )

install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications 
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
)

install ( FILES translations.ts/translations.ts DESTINATION share/qCheckGMail/translations.ts/ )
install ( FILES translations.ts/greek_GR.ts DESTINATION share/qCheckGMail/translations.ts/ )
install ( FILES translations.ts/dutch_NL.ts DESTINATION share/qCheckGMail/translations.ts/ )
install ( FILES translations.ts/german_DE.ts DESTINATION share/qCheckGMail/translations.ts/ )
install ( FILES translations.ts/spanish_ES.ts DESTINATION share/qCheckGMail/translations.ts/ )
install ( FILES translations.ts/french_FR.ts DESTINATION share/qCheckGMail/translations.ts/ )

install ( FILES translations.qm/greek_GR.qm DESTINATION share/qCheckGMail/translations.qm/ )
install ( FILES translations.qm/dutch_NL.qm DESTINATION share/qCheckGMail/translations.qm/ )
install ( FILES translations.qm/german_DE.qm DESTINATION share/qCheckGMail/translations.qm/ )
install ( FILES translations.qm/spanish_ES.qm DESTINATION share/qCheckGMail/translations.qm/ )
install ( FILES translations.qm/french_FR.qm DESTINATION share/qCheckGMail/translations.qm/ )

install ( FILES qCheckGMail.notifyrc DESTINATION ${DATA_INSTALL_DIR}/qCheckGMail/ )

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
Categories=Network;X-MandrivaLinux-Internet;\n")

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
Categories=Network;X-MandrivaLinux-Internet;\n")

