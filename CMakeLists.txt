# Copyright © 2018-2019 Pascal JEAN, All rights reserved.
# This file is part of the libmodbuspp Project.
#
# The libmodbuspp Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The libmodbuspp Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the libmodbuspp Project.  If not, see <http://www.gnu.org/licenses/>.

# modbuspp top-level CMakeLists.txt
cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (MODBUSPP)
set (PROJECT_DESCRIPTION "C++ wrapper for the libmodbus library")
set (PROJECT_DESCRIPTION_TEXT "\
  A C++ wrapper for the libmodbus library, to send/receive data with a device which\n\
  respects the Modbus protocol. This library can use a serial port or an\n\
  Ethernet connection."
)

# library for the Modbus protocol

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dev/cmake" "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_MODULE_PATH})

# Set a default build type if none was specified
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include/${PROJECT_NAME} CACHE PATH
  "Installation directory for header files")
set(INSTALL_CODELITE_DIR /usr/share/codelite CACHE PATH "Installation directory for codelite templates")

set(DEF_INSTALL_CMAKE_DIR lib)
set(DEF_INSTALL_DOC_DIR share/doc)
set(DEF_INSTALL_DATA_DIR share)

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
set(INSTALL_DOC_DIR ${DEF_INSTALL_DOC_DIR} CACHE PATH
  "Installation directory for documentation")
set(INSTALL_DATA_DIR ${DEF_INSTALL_DATA_DIR} CACHE PATH
  "Installation directory for data")

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules (LIBMODBUS REQUIRED libmodbus>=3.1.4)

find_package(Gzip)

set (MODBUSPP_CFLAGS_OTHER ${CMAKE_THREAD_LIBS_INIT} ${LIBMODBUS_CFLAGS})
set (MODBUSPP_LDFLAGS_OTHER ${LIBMODBUS_LDFLAGS} -lpthread)

include (GetDate)
GetDate(DATE_)

include (GitVersion)
GetGitVersion(MODBUSPP_VERSION)
set(MODBUSPP_VERSION
  ${MODBUSPP_VERSION_MAJOR}.${MODBUSPP_VERSION_MINOR}.${MODBUSPP_VERSION_PATCH})

include(CheckTypeSize)
check_type_size(bool SIZEOF_BOOL BUILTIN_TYPES_ONLY LANGUAGE CXX)
message(STATUS "  size of bool is ${SIZEOF_BOOL}")
if (NOT ${SIZEOF_BOOL} EQUAL 1)
  message(FATAL_ERROR "bool must have a size of 1 byte !")
endif()

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE DATA DOC CODELITE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

set (MODBUSPP_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set (MODBUSPP_INC_DIR ${PROJECT_SOURCE_DIR}/include)
set (MODBUSPP_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set (MODBUSPP_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/modbuspp/cmake")
set (MODBUSPP_INSTALL_DOC_DIR "${INSTALL_DOC_DIR}/modbuspp")
set (MODBUSPP_INSTALL_EXAMPLES_DIR "${MODBUSPP_INSTALL_DOC_DIR}/examples")
set (MODBUSPP_INSTALL_DATA_DIR "${INSTALL_DATA_DIR}/modbuspp")

message (STATUS "Building for ${CMAKE_SYSTEM_PROCESSOR} architecture.")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_definitions ( -DMODBUSPP_INSTALL_DATA_DIR="${MODBUSPP_INSTALL_DATA_DIR}" 
                  -DMODBUSPP_INSTALL_ETC_DIR="${MODBUSPP_INSTALL_ETC_DIR}")
                  
# Suppress GCC warnings
# See https://gcc.gnu.org/gcc-7/changes.html
# On ARM targets (arm*-*-*), a bug introduced in GCC 5 that affects conformance 
# to the procedure call standard (AAPCS) has been fixed. The bug affects some 
# C++ code where class objects are passed by value to functions and could result 
# in incorrect or inconsistent code being generated. This is an ABI change. 
# If the option -Wpsabi is enabled (on by default) the compiler will emit a 
# diagnostic note for code that might be affected. 
# <<< TODO >>> Fix this problem before compile with GCC 7 !
add_compile_options(-Wno-psabi)

add_subdirectory (dev)
add_subdirectory (lib)
#add_subdirectory (utils)
add_subdirectory (doc)

if (MODBUSPP_WITH_DOXYGEN_DOC)
  add_dependencies(modbuspp-shared doc)
endif()

### Packages generation --------------------------------------------------------
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "${MODBUSPP_VERSION_MAJOR}.${MODBUSPP_VERSION_MINOR}.${MODBUSPP_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MODBUSPP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MODBUSPP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MODBUSPP_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <pascal.jean@piduino.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - ${PROJECT_DESCRIPTION}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING.LESSER")

set(CPACK_GENERATOR "DEB")

if (CPACK_GENERATOR STREQUAL "DEB")

  set(CPACK_DEB_PACKAGE_COMPONENT 1)
  set(CPACK_DEB_COMPONENT_INSTALL 1)
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/epsilonrt/libmodbuspp")

  find_program(dpkg_query NAMES dpkg-query)
  find_program(dpkg_cmd NAMES dpkg)

  if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    if (dpkg_cmd)
      execute_process(COMMAND ${dpkg_cmd} --print-architecture RESULT_VARIABLE ret OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
      if (ret EQUAL 0)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arch}")
      else (ret EQUAL 0)
        message(WARNING "Unable to get debian architecture !")
      endif (ret EQUAL 0)
    endif (dpkg_cmd)
  endif (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)

  
  message(STATUS "Debian architecture ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  
  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libmodbuspp")
  set(CPACK_COMPONENT_LIB_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (library)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the shared libriairie itself.")
  set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libstdc++6,libmodbus5")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libmodbuspp-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (development files)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the development files.")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}),pkg-config,git-core,libmodbus-dev")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${MODBUSPP_SRC_DIR}/doc/README-deb.md")

  set(CPACK_DEBIAN_DOC_PACKAGE_NAME "libmodbuspp-doc")
  set(CPACK_COMPONENT_DOC_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (documentation)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the API documentation.")
  set(CPACK_DEBIAN_DOC_FILE_NAME "lib${PROJECT_NAME}-doc_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  #set(CPACK_DEBIAN_DOC_PACKAGE_DEPENDS "${CPACK_DEBIAN_DEV_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_DOC_PACKAGE_SECTION "libdevel")
    
endif (CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
