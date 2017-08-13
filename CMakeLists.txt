
cmake_minimum_required(VERSION 2.6)

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(qCheckGMail)

INCLUDE(GNUInstallDirs)

if( "${DATA_INSTALL_DIR}" STREQUAL "" )
	set( DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/" )
endif()

add_definitions( -Wno-unused-result -D_DEFAULT_SOURCE )

INCLUDE(CMakeDependentOption)
INCLUDE(FindPkgConfig)

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

INCLUDE(GNUInstallDirs)

set( QT5 "true" )

if( INTERNAL_LXQT_WALLET )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "lxqt wallet support will be provided by an internal library" )
	message( STATUS "---------------------------------------------------------------------------" )

	ADD_SUBDIRECTORY( lxqt_wallet )
	include_directories( ${PROJECT_SOURCE_DIR}/lxqt_wallet/frontend )
else()
	pkg_check_modules( LXQT_WALLET lxqt-wallet )

	if( NOT LXQT_WALLET_FOUND )

		message( STATUS "ERROR: could not find lxqt_wallet package" )
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "lxqt wallet support will be provided by an internal library" )
		message( STATUS "---------------------------------------------------------------------------" )

		ADD_SUBDIRECTORY( lxqt_wallet )
		include_directories( ${PROJECT_SOURCE_DIR}/lxqt_wallet/frontend )
	else()
		if( LXQT_WALLET_VERSION VERSION_GREATER "2.0.0" )

			include_directories( ${LXQT_WALLET_INCLUDEDIR} )
			link_directories( ${LXQT_WALLET_LIBDIR} )

			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an external libary" )
			message( STATUS "---------------------------------------------------------------------------" )
		else()
			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an internal library" )
			message( STATUS "because external library is too old(<= 2.0.0)" )

			message( STATUS "---------------------------------------------------------------------------" )

			ADD_SUBDIRECTORY( lxqt_wallet )
			include_directories( ${PROJECT_SOURCE_DIR}/lxqt_wallet/frontend )
		endif()
	endif()
endif()

include_directories( ${CMAKE_BINARY_DIR} )

add_definitions( -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR})
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

file( WRITE "${PROJECT_BINARY_DIR}/audio_file_path.h"
"\n#define AUDIO_NOTIFY_FILE \"${DATA_INSTALL_DIR}/qCheckGMail/audio/KDE-Im-New-Mail.ogg\"\n"
)

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

install( FILES qCheckGMail.notifyrc DESTINATION "${DATA_INSTALL_DIR}/apps/qCheckGMail/" )
install( FILES qCheckGMail.notifyrc DESTINATION "${DATA_INSTALL_DIR}/kde4/apps/qCheckGMail/" )
install( FILES qCheckGMail.notifyrc DESTINATION "${DATA_INSTALL_DIR}/knotifications5/" )

file( WRITE "${PROJECT_BINARY_DIR}/language_path.h"
"\n#define LANGUAGE_FILE_PATH \"${DATA_INSTALL_DIR}/qCheckGMail/\""
)

set( UI_FILES 	src/gmailauthorization.ui
		src/gmailauthorization.ui
		src/walletmanager.ui
		src/addaccount.ui
		src/configurationoptionsdialog.ui
)

set( MOC_FILES	src/gmailauthorization.h
		src/networkAccessManager.hpp
		src/qcheckgmail.h
		src/walletmanager.h
		src/addaccount.h
		src/configurationoptionsdialog.h
		src/statusicon.h
		src/task.h
)

set( CPP_FILES	src/gmailauthorization.cpp
		src/main.cpp
		src/qcheckgmail.cpp
		src/accounts.cpp
		src/statusicon.cpp
                src/walletmanager.cpp
		src/addaccount.cpp
		src/configurationoptionsdialog.cpp
)

set( RESOUCES 	icons/icon.qrc )

find_package( Qt5Widgets REQUIRED )
find_package( Qt5Core REQUIRED )
find_package( Qt5Network REQUIRED )

find_package( ECM REQUIRED NO_MODULE )
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

add_definitions( ${Qt5Widgets_DEFINITIONS} )

if( KF5 )
	find_package( KF5Config REQUIRED )
	find_package( KF5Notifications REQUIRED)
	find_package( KF5CoreAddons )
	find_package( KF5I18n NO_MODULE )

	message( STATUS "\n------------------------------------------------------------------------------------------" )
	message( STATUS "kde support found,will build KStatusNotifierItem based tray icon application" )
	message( STATUS "---------------------------------------------------------------------------------------" )

	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define KF5 1\n" )

	QT5_WRAP_CPP( MOC ${MOC_FILES} )
	QT5_WRAP_UI( UI ${UI_FILES} )
	QT5_ADD_RESOURCES( TRAY_RC_SRCS ${RESOUCES} )

	add_executable( qCheckGMail ${CPP_FILES} ${MOC} ${UI} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( qCheckGMail lxqt-wallet ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} -lQt5Network KF5::Notifications KF5::CoreAddons )
else()
	message( STATUS "\n-------------------------------------------------------------------------------------------" )
	message( STATUS "kde support NOT found,will build QSystemTrayIcon based tray icon application" )
	message( STATUS "----------------------------------------------------------------------------------------" )

	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define KF5 0\n" )

	QT5_WRAP_CPP( MOC ${MOC_FILES} )
	QT5_WRAP_UI( UI ${UI_FILES} )
	QT5_ADD_RESOURCES( TRAY_RC_SRCS ${RESOUCES} )

	add_executable( qCheckGMail ${CPP_FILES} ${MOC} ${UI} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( qCheckGMail lxqt-wallet ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} -lQt5Network )
endif()

set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )
set_target_properties( qCheckGMail PROPERTIES INSTALL_RPATH "${LIB_PATH}" )

install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION /etc/xdg/autostart/
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

#Icon=${CMAKE_INSTALL_PREFIX}/share/icons/qCheckGMail.svg

# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/qCheckGMail.desktop"
"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/qCheckGMail
GenericName[en_US]=a gmail new mail checker
GenericName=a gmail new mail checker
Icon=qCheckGMail
Name[en_US]=qCheckGMail
Name=qCheckGMail
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
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
Categories=Network;X-MandrivaLinux-Internet;\n")
