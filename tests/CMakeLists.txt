# CMakeLists.txt for ./tests directory
cmake_minimum_required(VERSION 3.10.0)

# Test configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages needed for testing
if(o2_WITH_QT5)
    find_package(Qt5 5.15 COMPONENTS Core Network Test REQUIRED)
else()
    find_package(Qt6 COMPONENTS Core Network Test REQUIRED)
endif()

# Source files from src directory
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/o0baseauth.cpp
    ${CMAKE_SOURCE_DIR}/src/o0jsonresponse.cpp
    ${CMAKE_SOURCE_DIR}/src/o2.cpp
    ${CMAKE_SOURCE_DIR}/src/o2pollserver.cpp
    ${CMAKE_SOURCE_DIR}/src/o2reply.cpp
    ${CMAKE_SOURCE_DIR}/src/o2replyserver.cpp
    ${CMAKE_SOURCE_DIR}/src/o2requestor.cpp
    ${CMAKE_SOURCE_DIR}/src/o0settingsstore.cpp
    ${CMAKE_SOURCE_DIR}/src/o2simplecrypt.cpp
)

# Header files from src directory
set(HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/o0abstractstore.h
    ${CMAKE_SOURCE_DIR}/src/o0baseauth.h
    ${CMAKE_SOURCE_DIR}/src/o2.h
    ${CMAKE_SOURCE_DIR}/src/o2pollserver.h
    ${CMAKE_SOURCE_DIR}/src/o2reply.h
    ${CMAKE_SOURCE_DIR}/src/o2replyserver.h
    ${CMAKE_SOURCE_DIR}/src/o2requestor.h
    ${CMAKE_SOURCE_DIR}/src/o0settingsstore.h
    ${CMAKE_SOURCE_DIR}/src/o0export.h
    ${CMAKE_SOURCE_DIR}/src/o0globals.h
    ${CMAKE_SOURCE_DIR}/src/o0jsonresponse.h
)

# Test utilities header
set(TEST_UTILS
    testutils.h
)

# Test files in current directory
set(TEST_FILES
    testbaseauth.cpp
    teststore.cpp
    testreplyserver.cpp
    testcrypt.cpp
    testo2.cpp
)

# Enable testing
enable_testing()

# Function to create test executable
function(add_oauth_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME}
        ${TEST_FILE}
        ${TEST_UTILS}
        ${SOURCE_FILES}
        ${HEADER_FILES}
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE
        Qt::Core
        Qt::Network
        Qt::Test
    )

    target_include_directories(${TEST_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_test(NAME ${TEST_NAME}
             COMMAND ${TEST_NAME}
             WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    set_target_properties(${TEST_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction()

# Create test executables
foreach(TEST_FILE ${TEST_FILES})
    add_oauth_test(${TEST_FILE})
endforeach()

# Add custom target to run all tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
