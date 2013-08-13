cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)


add_library( lxqtwallet STATIC lxqtwallet.c )
set_target_properties( lxqtwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wall -s -fPIC -pthread -pedantic " ) 
set_target_properties( lxqtwallet PROPERTIES LINK_FLAGS "-pie" )
TARGET_LINK_LIBRARIES( lxqtwallet "-lgcrypt" )
