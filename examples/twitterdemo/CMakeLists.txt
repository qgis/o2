cmake_minimum_required(VERSION 3.10.0)

project( twitterexample )

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
    tweeter.cpp
)

add_executable( twitterexample ${fb_SRCS} )

target_link_libraries( twitterexample Qt::Core Qt::Widgets Qt::Network )
target_link_libraries( twitterexample o2 )
