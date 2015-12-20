cmake_minimum_required(VERSION 2.8)
project(libraig)
include(ExternalProject)

ExternalProject_Add(libsocket
    PREFIX ${CMAKE_BINARY_DIR}/build/libsocket
    GIT_REPOSITORY https://github.com/damorton/libsocket.git
    INSTALL_COMMAND ""
    BINARY_DIR ${CMAKE_BINARY_DIR}/external/libsocket
    SOURCE_DIR ${CMAKE_BINARY_DIR}/external/libsocket
)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(SOURCES src/raig.cc)

if(UNIX AND NOT APPLE)
  set(PROJECT_LINK_LIBS libsocket.so)
else()
  set(PROJECT_LINK_LIBS libsocket.dylib)
endif()

link_directories(external/libsocket/)

add_library(raig SHARED ${SOURCES})

add_dependencies(raig libsocket)

target_link_libraries(raig ${PROJECT_LINK_LIBS})
