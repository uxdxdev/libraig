# /*
#
# The MIT License (MIT)
#
# Copyright (c) 2016 David Morton
#
# https://github.com/damorton/libraig.git
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# */
cmake_policy(SET CMP0017 NEW)
cmake_minimum_required(VERSION 2.8)
project(libraig)
include(ExternalProject)
include (GenerateExportHeader)


ExternalProject_Add(libsocket
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build/libsocket
    GIT_REPOSITORY https://github.com/damorton/libsocket.git
    GIT_TAG windows
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/libsocket
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/libsocket
    INSTALL_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)

include_directories(	
	#${CMAKE_CURRENT_SOURCE_DIR}/external/libcurl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src	
)
set(SOURCES 
	src/client/raig_client.cc 
	src/base/vector3.cc 
	src/net/net_manager.cc
	#src/http/http_client.cc
)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

link_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/external/libsocket/lib
	#${CMAKE_CURRENT_SOURCE_DIR}/external/libcurl/lib
)
add_library(raig SHARED ${SOURCES})
target_link_libraries(raig socket)

GENERATE_EXPORT_HEADER(raig
             BASE_NAME raig
             EXPORT_MACRO_NAME raig_EXPORT
             EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/raig/raig_Export.h
)

set_target_properties(raig
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)
