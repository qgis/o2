cmake_minimum_required(VERSION 3.10.0)

project( vimeoexample )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(o2_WITH_QT6)
    find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
else()
    find_package(Qt5 COMPONENTS Core Widgets Script Network REQUIRED)
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(vimeo_SRCS
    main.cpp
    vimeodemo.cpp
    vimeodemo.h
)

add_executable( vimeoexample ${vimeo_SRCS} )

target_link_libraries( vimeoexample Qt::Core Qt::Widgets Qt::Network )
target_link_libraries( vimeoexample o2 )
