cmake_minimum_required(VERSION 2.6)

INCLUDE(FindPkgConfig)

find_file( GCRYPT_INCLUDE_FILE gcrypt.h )

find_library( GCRYPT_LIBRARY gcrypt )

add_definitions( -Wall -pedantic -Wformat-truncation=0 )

if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "could not find gcrypt header file" )
else()
	MESSAGE( STATUS "found gcrypt header file: ${GCRYPT_INCLUDE_FILE}" )
endif()

if( NOT GCRYPT_LIBRARY )
	MESSAGE( FATAL_ERROR "could not find gcrypt library(libgcrypt.so)" )
else()
	MESSAGE( STATUS "found gcrypt library: ${GCRYPT_LIBRARY}" )
endif()

add_library( lxqt-wallet-backend STATIC lxqtwallet.c )
set_target_properties( lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-s -fPIC" )
set_target_properties( lxqt-wallet-backend PROPERTIES LINK_FLAGS "-pie" )
TARGET_LINK_LIBRARIES( lxqt-wallet-backend "${GCRYPT_LIBRARY}" )
