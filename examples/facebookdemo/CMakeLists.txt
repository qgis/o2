cmake_minimum_required(VERSION 3.10.0)

project( fbexample )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(o2_WITH_QT6)
    find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
else()
    find_package(Qt5 COMPONENTS Core Widgets Script Network REQUIRED)
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(fb_SRCS
    main.cpp
    fbdemo.cpp
    fbdemo.h
)

add_executable( fbexample ${fb_SRCS} )

target_link_libraries( fbexample Qt::Core Qt::Widgets Qt::Network )
target_link_libraries( fbexample o2 )
