
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

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


if( NOKDESUPPORT ) 
	SET( KDE_SUPPORT "false" )
	SET( KWALLET_SUPPORT "false" )
	file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
else()
	SET( KDE_SUPPORT "true" )
	
	if( NOKWALLETSUPPORT )
		SET( KWALLET_SUPPORT "false" )	
		file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
	else()	
		find_library( LIBKWALLETBACKEND libkwalletbackend.so )
		if( NOT LIBKWALLETBACKEND )
			# some distributions do not have libkwalletbackend.so for reasons i currently do not know
			# look for libkwalletbackend.sp.4 when the above happen
			find_library( LIBKWALLETBACKEND libkwalletbackend.so.4 )
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
			SET( KWALLET_SUPPORT "false" )	
		else()
			SET( KWALLET_SUPPORT "true" )
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n" )
		endif()

		if( NOT LIBKWALLETBACKEND )
			SET( KWALLET_SUPPORT "false" )	
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
			#message( FATAL_ERROR "could not find libkwalletbackend.so or libkwalletbackend.so.4,please make sure you have kwallet development libraries installed and try again" )
		else()
			SET( KWALLET_SUPPORT "true" )	
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n" )
		endif()
	endif()
	
	find_library( LIBKDEUI libkdeui.so )
	if( NOT LIBKDEUI )
		# adding this one just in case the same case as above happen
		find_library( LIBKDEUI libkdeui.so.5 )
	endif()
	
	if( NOT LIBKDEUI )
		SET( KDE_SUPPORT "false" )
		message( STATUS "could not find libkdeui.so or libkdeui.so.5,please make sure you have kde development libraries installed and try again" )
	endif()

	find_library( LIBKDECORE libkdecore.so )
	if( NOT LIBKDECORE )
		# adding this one just in case the same case as above happen	
		find_library( LIBKDECORE libkdecore.so.5 )
	endif()

	if( NOT LIBKDECORE )
		SET( KDE_SUPPORT "false" )
		message( STATUS "could not find libkdecore.so or libkdecore.so.5,please make sure you have kde development libraries installed and try again" )
	endif()
endif()

if( KWALLET_SUPPORT )
	SET( KWALLET_1 "storage_backends/lxqt_kwallet.h" )
	SET( KWALLET_2 "storage_backends/lxqt_kwallet.cpp" )
	message( STATUS "\n--------------------------------------------------------------------------" )
	message( STATUS "kwallet support found,account credentials will be stored in KWallet" )
	message( STATUS "-----------------------------------------------------------------------" )
else()
	SET( KWALLET_1 "" )
	SET( KWALLET_2 "" )
	message( STATUS "\n--------------------------------------------------------------------------" )
	message( STATUS "kwallet support NOT found,account credentials will be stored with an internal system" )
	message( STATUS "-----------------------------------------------------------------------" )
endif()	

QT4_WRAP_CPP( MOC qcheckgmail.h walletmanager.h addaccount.h configurationoptionsdialog.h 
storage_backends/lxqt_internal_wallet.h ${KWALLET_1} storage_backends/password_dialog.h
storage_backends/lxqt_wallet_interface.h storage_backends/open_wallet_thread.h
)

QT4_WRAP_UI( UI walletmanager.ui addaccount.ui configurationoptionsdialog.ui storage_backends/password_dialog.ui )

add_executable( qCheckGMail main.cpp qcheckgmail.cpp accounts.cpp storage_backends/lxqt_internal_wallet.cpp 
storage_backends/lxqt_wallet_interface.cpp storage_backends/open_wallet_thread.cpp  ${KWALLET_2}
storage_backends/password_dialog.cpp storage_backends/lxqt_wallet_interface.cpp
walletmanager.cpp addaccount.cpp configurationoptionsdialog.cpp ${MOC} ${UI} ${ICONS} )

if( KDE_SUPPORT )
	message( STATUS "\n--------------------------------------------------------------------------" )
	message( STATUS "kde support found,will build a version that uses kde status notifier" )
	message( STATUS "-----------------------------------------------------------------------" )
	#
	# If linking failed because of missing library,add the library at the end of below line inside the brackets
	#
	TARGET_LINK_LIBRARIES( qCheckGMail -L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE} ${LIBKWALLETBACKEND} -lQtCore -lQtGui -lQtNetwork lxqtwallet ) 
else()
	message( STATUS "\n--------------------------------------------------------------------------" )
	message( STATUS "kde support NOT found,will build a version with no kde support" )
	message( STATUS "-----------------------------------------------------------------------" )
	TARGET_LINK_LIBRARIES( qCheckGMail -L${QT_LIBRARY_DIR} -lQtCore -lQtGui -lQtNetwork lxqtwallet ) 
endif()	

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

