cmake_minimum_required(VERSION 3.0)

# 
project(callback-server-json)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(PkgConfig REQUIRED) 
pkg_check_modules(LIBMODBUSPP REQUIRED IMPORTED_TARGET modbuspp)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LIBMODBUSPP)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
