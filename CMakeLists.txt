cmake_minimum_required(VERSION 2.8.4)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(easyflags)

set(SOURCE_DIR                "src/")
set(INCLUDE_DIR               "include/")

set(CMAKE_CXX_STANDARD 11)

set(LIB_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
set(BIN_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

# thirdparty
add_definitions(-Dautojsonuselib)
if (NOT TARGET autojson)
    add_subdirectory(thirdparty/autojson)
endif ()

# library
include_directories(${INCLUDE_DIR})
add_library(${PROJECT_NAME}
    "${SOURCE_DIR}/EasyFlags.cpp"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC -D${PROJECT_NAME}uselib)
target_link_libraries(${PROJECT_NAME} autojson)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

add_executable(example-1 "examples/ex1.cpp")
target_link_libraries(example-1 PUBLIC ${PROJECT_NAME})
install(TARGETS example-1 DESTINATION example1)

# INTERFACE + LIBRARY
set(INCLUDE_INSTALL_DIR       "include")
set(SOURCE_INSTALL_DIR        "${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}_src")

install(
    DIRECTORY ${INCLUDE_DIR}
    DESTINATION ${INCLUDE_INSTALL_DIR}
)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION lib
)

# INTERFACE ONLY
install(
    DIRECTORY ${SOURCE_DIR}
    DESTINATION ${SOURCE_INSTALL_DIR}
)
