if(${CMAKE_VERSION} VERSION_GREATER "3.31.0" OR ${CMAKE_VERSION} VERSION_EQUAL "3.31.0")
    cmake_minimum_required(VERSION 3.31.0)
    cmake_policy(SET CMP0177 OLD)
else()
    cmake_minimum_required(VERSION 3.5.0)
endif()

project(qCheckGMail)

INCLUDE(GNUInstallDirs)

if( "${DATA_INSTALL_DIR}" STREQUAL "" )
	set( DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/" )
endif()

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

set( PGR_VERSION "Version:\t2.2.0" )
set( COPYRIGHT_YEARS "Copyright:\t2013-2026" )
set( LICENSE "License:\tGPLv2+" )

file(WRITE ${PROJECT_BINARY_DIR}/version.h
"
#define VERSION \"${PGR_VERSION}\"
#define COPYRIGHT \"${COPYRIGHT_YEARS}\"
#define LICENCE \"${LICENSE}\"
")

if( INTERNAL_LXQT_WALLET )

	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "lxqt wallet support will be provided by an internal library" )
	message( STATUS "---------------------------------------------------------------------------" )

	ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
	include_directories( src/3rdParty/lxqt_wallet/frontend )
else()
	pkg_check_modules( LXQT_WALLET lxqt-wallet )

	if( NOT LXQT_WALLET_FOUND )

		message( STATUS "ERROR: could not find lxqt_wallet package" )
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "lxqt wallet support will be provided by an internal library" )
		message( STATUS "---------------------------------------------------------------------------" )

		ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
		include_directories( src/3rdParty/lxqt_wallet/frontend )
	else()
	        if( LXQT_WALLET_VERSION VERSION_GREATER "3.1.0" )

			include_directories( ${LXQT_WALLET_INCLUDEDIR} )
			link_directories( ${LXQT_WALLET_LIBDIR} )

			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an external library" )
			message( STATUS "---------------------------------------------------------------------------" )
		else()
			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an internal library" )
			message( STATUS "because external library is too old(< 3.2.0)" )
			message( STATUS "\nPlease install a more receant version from: https://github.com/lxqt/lxqt_wallet
			message" )

			message( STATUS "---------------------------------------------------------------------------" )

			ADD_SUBDIRECTORY( src/3rdParty/lxqt_wallet )
			include_directories( src/3rdParty/lxqt_wallet/frontend )
		endif()
	endif()
endif()

include_directories( ${CMAKE_BINARY_DIR} )

add_definitions( -Wno-unused-result -D_DEFAULT_SOURCE )

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-Qunused-arguments)
endif()

if( ${CMAKE_VERSION} VERSION_LESS "3.1.0" )
    message("Warning your cmake version is too old(<3.1.0) and code my not compile")
    add_definitions(-std=c++14)
endif()

add_definitions( -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR})
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set(CMAKE_CXX_EXTENSIONS OFF)

file( WRITE "${PROJECT_BINARY_DIR}/audio_file_path.h"
"\n#define AUDIO_NOTIFY_FILE \"${DATA_INSTALL_DIR}/qCheckGMail/audio/new-email-sound.ogg\"\n"
)

file( WRITE "${PROJECT_BINARY_DIR}/icon_file_path.h"
"\n#define ICON_FILE_PATH \"${DATA_INSTALL_DIR}/icons/qCheckGMail-hasMail.svg\"\n"
)

install( FILES audio/new-email-sound.ogg     DESTINATION "${DATA_INSTALL_DIR}/qCheckGMail/audio" )

install( FILES icons/qCheckGMail.svg           DESTINATION ${DATA_INSTALL_DIR}/icons )
install( FILES icons/qCheckGMail-hasNoMail.svg DESTINATION ${DATA_INSTALL_DIR}/icons )
install( FILES icons/qCheckGMail-hasError.svg  DESTINATION ${DATA_INSTALL_DIR}/icons )
install( FILES icons/qCheckGMail-hasMail.svg   DESTINATION ${DATA_INSTALL_DIR}/icons )
install( FILES icons/qCheckGMail-paused.svg   DESTINATION ${DATA_INSTALL_DIR}/icons )

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

install( FILES src/3rdParty/lxqt_wallet/translations.qm/german_DE.qm   DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES src/3rdParty/lxqt_wallet/translations.qm/greek_GR.qm    DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES src/3rdParty/lxqt_wallet/translations.qm/dutch_NL.qm    DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES src/3rdParty/lxqt_wallet/translations.qm/french_FR.qm   DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )
install( FILES src/3rdParty/lxqt_wallet/translations.qm/spanish_ES.qm   DESTINATION share/qCheckGMail/lxqt_wallet/translations.qm/ )

file( WRITE "${PROJECT_BINARY_DIR}/language_path.h"
"\n#define LANGUAGE_FILE_PATH \"${DATA_INSTALL_DIR}/qCheckGMail/\""
)

find_package( Qt6Widgets REQUIRED )
find_package( Qt6Core REQUIRED )
find_package( Qt6Network REQUIRED )
find_package( Qt6DBus REQUIRED)


set( UI_FILES
	src/gmailauthorization.ui
	src/gmailauthorization.ui
	src/walletmanager.ui
	src/addaccount.ui
	src/logwindow.ui
	src/configurationoptionsdialog.ui
)

set( MOC_FILES
		src/statusicon.h
		src/walletmanager.h
		src/addaccount.h
		src/qcheckgmail.h
		src/logwindow.h
		src/configurationoptionsdialog.h )

set( CPP_FILES
	src/gmailauthorization.cpp
	src/main.cpp
	src/settings.cpp
	src/qcheckgmail.cpp
	src/accounts.cpp
	src/statusicon.cpp
	src/logwindow.cpp
	src/walletmanager.cpp
	src/addaccount.cpp
	src/configurationoptionsdialog.cpp
)

set( RESOUCES 	icons/icon.qrc )

#find_package( ECM REQUIRED NO_MODULE )
#set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

set( CMAKE_INCLUDE_CURRENT_DIR ON )

include_directories( ${Qt6Widgets_INCLUDE_DIRS} )
include_directories( ${Qt6DBus_INCLUDE_DIRS} )
include_directories( ${Qt6Network_INCLUDE_DIRS} )

add_definitions( ${Qt6Widgets_DEFINITIONS} )

set( KF6 "true" )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

if( KF6 )
	cmake_policy( SET CMP0012 NEW )
	find_package( KF6Config REQUIRED )
	find_package( KF6Notifications REQUIRED )
	find_package( KF6CoreAddons )
	find_package( KF6I18n NO_MODULE )

	message( STATUS "\n------------------------------------------------------------------------------------------" )
	message( STATUS "kde support found,will build KStatusNotifierItem based tray icon application" )
	message( STATUS "---------------------------------------------------------------------------------------" )

	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define KF6 1\n" )

	SET( MOC ${MOC_FILES} )
	SET( UI ${UI_FILES} )
	SET( TRAY_RC_SRCS ${RESOUCES} )

	add_executable( qCheckGMail ${CPP_FILES} ${MOC} ${UI} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( qCheckGMail lxqt-wallet ${Qt6Widgets_LIBRARIES} ${Qt6Core_LIBRARIES} ${Qt6Network_LIBRARIES} KF6::Notifications KF6::CoreAddons )
else()
	message( STATUS "\n-------------------------------------------------------------------------------------------" )
	message( STATUS "kde support NOT found,will build QSystemTrayIcon based tray icon application" )
	message( STATUS "----------------------------------------------------------------------------------------" )

	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define KF6 0\n" )

	SET( MOC ${MOC_FILES} )
	SET( UI ${UI_FILES} )
	SET( TRAY_RC_SRCS ${RESOUCES} )

	add_executable( qCheckGMail ${CPP_FILES} ${MOC} ${UI} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( qCheckGMail lxqt-wallet ${Qt6Widgets_LIBRARIES} ${Qt6Core_LIBRARIES} ${Qt6Network_LIBRARIES} ${Qt6DBus_LIBRARIES})
endif()

set_target_properties( qCheckGMail PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
set_target_properties( qCheckGMail PROPERTIES LINK_FLAGS "-pie" )
set_target_properties( qCheckGMail PROPERTIES INSTALL_RPATH "${LIB_PATH}" )

install( TARGETS qCheckGMail RUNTIME DESTINATION bin )

install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail.desktop DESTINATION share/applications
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install( FILES ${PROJECT_BINARY_DIR}/qCheckGMail-autostart.desktop DESTINATION etc/xdg/autostart/
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
Categories=Network;Email;\n")

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
