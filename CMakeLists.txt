// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.

cmake_policy(SET CMP0017 NEW)
cmake_minimum_required(VERSION 2.8)
project(libraig)
include (GenerateExportHeader)

include_directories(	
	#${CMAKE_CURRENT_SOURCE_DIR}/external/libcurl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src	
)
set(SOURCES 
    include/export/raig_Export.h
    include/raig/raig_client.h
    include/base/vector3.h
    src/base/event.h 	
    src/base/io_buffer.h 	
    src/base/node.h 	
    src/base/observer.h 	
    src/http/http_client.h
    src/net/net_manager.h
    
    src/client/raig_client.cc    
	src/base/event.cc	
	src/base/vector3.cc 
	src/base/io_buffer.cc 
	src/net/net_manager.cc
	src/http/http_client.cc
)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(WIN32)
  set(CMAKE_C_FLAGS "/ZW")
endif()

link_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/external/libsocket/lib/linux
	${CMAKE_CURRENT_SOURCE_DIR}/external/libsocket/lib/win32
	#${CMAKE_CURRENT_SOURCE_DIR}/external/libcurl/lib
)
add_library(raig SHARED ${SOURCES})
target_link_libraries(raig socket)

GENERATE_EXPORT_HEADER(raig
             BASE_NAME raig
             EXPORT_MACRO_NAME raig_EXPORT
             EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/export/raig_Export.h
)

set_target_properties(raig
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)