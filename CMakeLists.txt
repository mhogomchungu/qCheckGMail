
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

if( LXQTSUPPORT )
	set( NOKDESUPPORT "true" )
endif()

ADD_SUBDIRECTORY( storage_backends )

find_package( Qt4 COMPONENTS REQUIRED )

find_package( KDE4 )
include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})
add_definitions( -I${KDE4_INCLUDE_DIR} )

add_definitions( -I${Qt4_INCLUDE_DIR} )
add_definitions( -I${QT_HEADERS_DIR} )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

install ( FILES qCheckGMail-GotMail.svg DESTINATION share/icons )
install ( FILES qCheckGMail-NoMail.svg DESTINATION share/icons )
install ( FILES qCheckGMail.svg DESTINATION share/icons )
install ( FILES qCheckGMailError.svg DESTINATION share/icons )

#install ( FILES translations.ts/translations.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install ( FILES translations.ts/greek_GR.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install ( FILES translations.ts/dutch_NL.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install ( FILES translations.ts/german_DE.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install ( FILES translations.ts/spanish_ES.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install ( FILES translations.ts/french_FR.ts DESTINATION share/qCheckGMail/translations.ts/ )

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

if( LXQTSUPPORT )
	INCLUDE(FindPkgConfig)

	pkg_check_modules( LXQT razorqt )
	if( LXQT_FOUND )

	QT4_WRAP_CPP( MOC qcheckgmail.h walletmanager.h addaccount.h configurationoptionsdialog.h statusicon.h qcheckgmailplugin.h )

	QT4_WRAP_UI( UI walletmanager.ui addaccount.ui configurationoptionsdialog.ui )

	QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

		add_library( qCheckGMail MODULE qcheckgmail.cpp accounts.cpp statusicon.cpp qcheckgmailplugin.cpp
			walletmanager.cpp addaccount.cpp configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

		set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
		set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )

		TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet -lQtNetwork )

		install( TARGETS qCheckGMail LIBRARY DESTINATION lib/razor-panel/ )

		install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/razor/razor-panel/
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		message( STATUS "\n------------------------------------------------------------------------------------------" )
		message( STATUS "lxqt support found,will build lxqt panel plugin based application" )
		message( STATUS "------------------------------------------------------------------------------------------" )
		file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_LXQT_PLUGIN 1\n#define USE_KDE_STATUS_NOTIFIER 0\n" )
	else()
		message( FATAL_ERROR "razor-qt/lxqt libraries not found" )
	endif()
elseif( NOKDESUPPORT )

	QT4_WRAP_CPP( MOC qcheckgmail.h walletmanager.h addaccount.h configurationoptionsdialog.h statusicon.h )

	QT4_WRAP_UI( UI walletmanager.ui addaccount.ui configurationoptionsdialog.ui )

	QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

	add_executable( qCheckGMail main.cpp qcheckgmail.cpp accounts.cpp statusicon.cpp
		walletmanager.cpp addaccount.cpp configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

	set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
	set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )

	TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet -lQtNetwork )

	install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

	install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	message( STATUS "\n-------------------------------------------------------------------------------------------" )
	message( STATUS "kde support NOT found,will build QSystemTrayIcon based tray icon application" )
	message( STATUS "-------------------------------------------------------------------------------------------" )
	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 0\n#define USE_LXQT_PLUGIN 0\n" )
else()

	QT4_WRAP_CPP( MOC qcheckgmail.h walletmanager.h addaccount.h configurationoptionsdialog.h statusicon.h )

	QT4_WRAP_UI( UI walletmanager.ui addaccount.ui configurationoptionsdialog.ui )

	QT4_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

	add_executable( qCheckGMail main.cpp qcheckgmail.cpp accounts.cpp statusicon.cpp
		walletmanager.cpp addaccount.cpp configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

	set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
	set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )

	TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet -lQtNetwork )

	install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

	install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	install ( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	message( STATUS "\n------------------------------------------------------------------------------------------" )
	message( STATUS "kde support found,will build KStatusNotifierItem based tray icon application" )
	message( STATUS "------------------------------------------------------------------------------------------" )
	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 1\n#define USE_LXQT_PLUGIN 0\n" )
endif()

if( LXQTSUPPORT )
# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/qCheckGMail.desktop"
"[Desktop Entry]
Type=Service
ServiceTypes=RazorPanel/Plugin
Name=qChechGMailPlugIn
Comment=a plugin to check for mail in gmail accounts\n")

else()
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
endif()
