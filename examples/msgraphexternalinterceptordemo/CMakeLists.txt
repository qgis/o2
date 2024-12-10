cmake_minimum_required(VERSION 3.10.0)

project( msgraphexternalinterceptorexample )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(o2_WITH_QT6)
    find_package(Qt6 COMPONENTS Core Widgets Network WebEngineCore WebEngineWidgets REQUIRED)
else()
    find_package(Qt5 COMPONENTS Core Widgets Script Network WebEngine WebEngineWidgets REQUIRED)
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(msgraph_SRCS
    main.cpp
    msgraphdemo.cpp
    webwindow.cpp
    webenginepage.cpp
)

add_executable( msgraphexternalinterceptorexample ${msgraph_SRCS} )

target_link_libraries( msgraphexternalinterceptorexample Qt::Core Qt::Widgets Qt::Network Qt::WebEngineWidgets )
target_link_libraries( msgraphexternalinterceptorexample o2 )
