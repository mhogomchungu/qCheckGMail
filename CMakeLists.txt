
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

add_definitions( -Wno-unused-result )

if( LXQTSUPPORT )
	set( NOKDESUPPORT "true" )
endif()

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

add_definitions( -I"${PROJECT_BINARY_DIR}/lxqt_wallet/frontend" )

ADD_SUBDIRECTORY( lxqt_wallet )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_definitions( -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR})

if( QT5 )
	set( NOKDESUPPORT "true" )

	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Core REQUIRED )

	set( CMAKE_INCLUDE_CURRENT_DIR ON )
	include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

	add_definitions( ${Qt5Widgets_DEFINITIONS} )
else()
	find_package( Qt4 REQUIRED )
	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )
	INCLUDE( ${QT_USE_FILE} )
endif()

if( NOT "${DATA_INSTALL_DIR}" )
	set( DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share" )
endif()

#install( FILES icons/qCheckGMail-GotMail.svg DESTINATION "${DATA_INSTALL_DIR}//pixmaps" )
#install( FILES icons/qCheckGMail-NoMail.svg  DESTINATION "${DATA_INSTALL_DIR}//pixmaps" )
install( FILES icons/qCheckGMail.svg         DESTINATION "${DATA_INSTALL_DIR}/pixmaps" )
install( FILES icons/qCheckGMail.svg         DESTINATION "${DATA_INSTALL_DIR}/icons" )

#install( FILES icons/qCheckGMailError.svg    DESTINATION "${DATA_INSTALL_DIR}//pixmaps" )

#install( FILES translations.ts/translations.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install( FILES translations.ts/greek_GR.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install( FILES translations.ts/dutch_NL.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install( FILES translations.ts/german_DE.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install( FILES translations.ts/spanish_ES.ts DESTINATION share/qCheckGMail/translations.ts/ )
#install( FILES translations.ts/french_FR.ts DESTINATION share/qCheckGMail/translations.ts/ )

install( FILES translations.qm/greek_GR.qm   DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/translations.qm/" )
install( FILES translations.qm/dutch_NL.qm   DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/translations.qm/" )
install( FILES translations.qm/german_DE.qm  DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/translations.qm/" )
install( FILES translations.qm/spanish_ES.qm DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/translations.qm/" )
install( FILES translations.qm/french_FR.qm  DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/translations.qm/" )

install( FILES lxqt_wallet/translations.qm/german_DE.qm   DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES lxqt_wallet/translations.qm/greek_GR.qm    DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES lxqt_wallet/translations.qm/dutch_NL.qm    DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES lxqt_wallet/translations.qm/french_FR.qm   DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES lxqt_wallet/translations.qm/spanish_ES.qm   DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )

install( FILES qCheckGMail.notifyrc DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/" )
#install( FILES qCheckGMail.notifyrc DESTINATION "/usr/share/apps/qCheckGMail/" )

file( WRITE "${PROJECT_BINARY_DIR}/language_path.h"
"\n#define LANGUAGE_FILE_PATH \"${DATA_INSTALL_DIR}/qCheckGMail/\""
)

if( LXQTSUPPORT )
	INCLUDE(FindPkgConfig)

	pkg_check_modules( LXQT lxqt )
	if( LXQT_FOUND )

		add_definitions( -I${LXQT_INCLUDE_DIR})
		include_directories( ${LXQT_INCLUDE_DIRS} )

		if( QT5 )
			QT5_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/qcheckgmailplugin.h src/task.h )
			QT5_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
			QT5_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

			add_library( qCheckGMail MODULE src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/qcheckgmailplugin.cpp src/task.cpp
			src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

			TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} -lQt5Network ${LXQT_LIBRARIES})
		else()
			QT4_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/qcheckgmailplugin.h src/task.h )
			QT4_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
			QT4_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

			add_library( qCheckGMail MODULE src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/qcheckgmailplugin.cpp src/task.cpp
			src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

			TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${QT_LIBRARIES} -lQtNetwork ${LXQT_LIBRARIES})
		endif()

		set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
		set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )
		set_target_properties( qCheckGMail PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/" )

		install( TARGETS qCheckGMail LIBRARY DESTINATION lib/lxqt-panel/ )

		install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/lxqt/lxqt-panel/
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		message( STATUS "\n------------------------------------------------------------------------------------------" )
		message( STATUS "lxqt support found,will build lxqt panel plugin based application" )
		message( STATUS "------------------------------------------------------------------------------------------" )
		file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_LXQT_PLUGIN 1\n#define USE_KDE_STATUS_NOTIFIER 0\n" )
	else()
		message( FATAL_ERROR "lxqt libraries not found" )
	endif()
elseif( NOKDESUPPORT )

	if( QT5 )
		QT5_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/task.h )
		QT5_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
		QT5_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

		add_executable( qCheckGMail src/main.cpp src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/task.cpp
		src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

		TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} -lQt5Network )
	else()
		QT4_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/task.h )
		QT4_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
		QT4_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

		add_executable( qCheckGMail src/main.cpp src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/task.cpp
		src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

		TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${QT_LIBRARIES} -lQtNetwork )
	endif()

	set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
	set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )
	set_target_properties( qCheckGMail PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/" )

	install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

	install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	message( STATUS "\n-------------------------------------------------------------------------------------------" )
	message( STATUS "kde support NOT found,will build QSystemTrayIcon based tray icon application" )
	message( STATUS "----------------------------------------------------------------------------------------" )
	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 0\n#define USE_LXQT_PLUGIN 0\n" )
else()
	find_package( KDE4 QUIET )
	if( KDE4_FOUND )
		include (KDE4Defaults)
		include_directories(${KDE4_INCLUDES})
		add_definitions( -I${KDE4_INCLUDE_DIR} )

		find_library( LIBKDEUI_ libkdeui.so PATHS ${KDE_LIBRARY_PATH} )
		if( NOT LIBKDEUI_ )
			# adding this one just in case the same case as above happen
			find_library( LIBKDEUI_ libkdeui.so.5 )
		endif()

		if( NOT LIBKDEUI_ )
			SET( KDE_SUPPORT "false" )
			message( FATAL_ERROR "could not find libkdeui.so or libkdeui.so.5,please make sure you have kde development libraries installed and try again" )
		endif()

		find_library( LIBKDECORE_ libkdecore.so PATHS ${KDE_LIBRARY_PATH} )
		if( NOT LIBKDECORE_ )
			# adding this one just in case the same case as above happen
			find_library( LIBKDECORE_ libkdecore.so.5 )
		endif()

		if( NOT LIBKDECORE_ )
			SET( KDE_SUPPORT "false" )
			message( FATAL_ERROR "could not find libkdecore.so or libkdecore.so.5,please make sure you have kde development libraries installed and try again" )
		endif()

		if( QT5 )
			QT5_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/task.h )
			QT5_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
			QT5_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

			add_executable( qCheckGMail src/main.cpp src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/task.cpp
			src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

			TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} -lQt5Network )
		else()
			QT4_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/task.h )
			QT4_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
			QT4_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

			add_executable( qCheckGMail src/main.cpp src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/task.cpp
			src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

			TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${QT_LIBRARIES} -lQtNetwork )
		endif()

		set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
		set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )
		set_target_properties( qCheckGMail PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/" )

		install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

		install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		message( STATUS "\n------------------------------------------------------------------------------------------" )
		message( STATUS "kde support found,will build KStatusNotifierItem based tray icon application" )
		message( STATUS "---------------------------------------------------------------------------------------" )
		file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 1\n#define USE_LXQT_PLUGIN 0\n" )
	else()
		if( QT5 )
			QT5_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/task.h )
			QT5_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
			QT5_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

			add_executable( qCheckGMail src/main.cpp src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/task.cpp
			src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

			TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} -lQt5Network )
		else()
			QT4_WRAP_CPP( MOC src/qcheckgmail.h src/walletmanager.h src/addaccount.h src/configurationoptionsdialog.h src/statusicon.h src/task.h )
			QT4_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
			QT4_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

			add_executable( qCheckGMail src/main.cpp src/qcheckgmail.cpp src/accounts.cpp src/statusicon.cpp src/task.cpp
			src/walletmanager.cpp src/addaccount.cpp src/configurationoptionsdialog.cpp ${MOC} ${UI} ${TRAY_RC_SRCS} )

			TARGET_LINK_LIBRARIES( qCheckGMail lxqtwallet ${QT_LIBRARIES} -lQtNetwork )
		endif()

		set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
		set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )
		set_target_properties( qCheckGMail PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/" )

		install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

		install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		message( STATUS "\n-------------------------------------------------------------------------------------------" )
		message( STATUS "kde support NOT found,will build QSystemTrayIcon based tray icon application" )
		message( STATUS "------------------------------------------------------------------------------------------" )
		file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 0\n#define USE_LXQT_PLUGIN 0\n" )
	endif()
endif()

#Icon=${CMAKE_INSTALL_PREFIX}/share/icons/qCheckGMail.svg

if( LXQTSUPPORT )
# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/qCheckGMail.desktop"
"[Desktop Entry]
Type=Service
ServiceTypes=LxQtPanel/Plugin
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
Icon=qCheckGMail
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
Icon=qCheckGMail
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
