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

project (MODBUSPP LANGUAGES CXX)
set (PROJECT_DESCRIPTION "Much more than a C++ wrapper for the libmodbus library")
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

set (MODBUSPP_WITH_STATIC 0 CACHE BOOL "Enable building static libraries.")
set (MODBUSPP_UNIT_TESTS 1 CACHE BOOL "Enable building unit tests.")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
set(INSTALL_CODELITE_DIR /usr/share/codelite CACHE PATH "Installation directory for codelite templates")
option(MODBUSPP_USE_EXTERNAL_JSON "Use an external JSON library" OFF)

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

set(LIBMODBUS_MIN_VERSION "3.1.6")
find_package(PkgConfig REQUIRED)

pkg_check_modules (LIBMODBUS libmodbusepsi>=${LIBMODBUS_MIN_VERSION})
if (LIBMODBUS_FOUND)
  set(LIBMODBUS_NAME modbusepsi)
else()
  pkg_check_modules (LIBMODBUS REQUIRED libmodbus>=${LIBMODBUS_MIN_VERSION})
  set(LIBMODBUS_NAME modbus)
endif()

set(LIBMODBUS_PACKAGE lib${LIBMODBUS_NAME}5)
set(LIBMODBUSDEV_PACKAGE lib${LIBMODBUS_NAME}-dev)

find_package(Gzip)

pkg_check_modules (UNITTESTPP UnitTest++>=2.0.0)

if(MODBUSPP_USE_EXTERNAL_JSON)
  find_package(nlohmann_json 3.2.0 REQUIRED)
else()
  set(JSON_BuildTests OFF CACHE INTERNAL "")
  set(JSON_Install OFF CACHE INTERNAL "")
  add_subdirectory(3rdparty/json)
endif()

set (MODBUSPP_CFLAGS_OTHER ${CMAKE_THREAD_LIBS_INIT} ${LIBMODBUS_CFLAGS})
set (MODBUSPP_LDFLAGS_OTHER ${LIBMODBUS_LDFLAGS} -lpthread)

include (GetDate)
GetDate(DATE_)

include (GitVersion)
GetGitVersion(MODBUSPP_VERSION)
set(MODBUSPP_VERSION
  ${MODBUSPP_VERSION_MAJOR}.${MODBUSPP_VERSION_MINOR}.${MODBUSPP_VERSION_PATCH})
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)

include(CheckTypeSize)
check_type_size(bool SIZEOF_BOOL BUILTIN_TYPES_ONLY LANGUAGE CXX)
message(STATUS "  size of bool is ${SIZEOF_BOOL}")
if (NOT ${SIZEOF_BOOL} EQUAL 1)
  message(FATAL_ERROR "bool must have a size of 1 byte !")
endif()

include(CheckSymbolExists)
check_symbol_exists(TIOCSRS485 sys/ioctl.h MODBUSPP_HAVE_TIOCRS485)
check_symbol_exists(TIOCM_RTS sys/ioctl.h MODBUSPP_HAVE_TIOCM_RTS)

list(APPEND CMAKE_REQUIRED_LIBRARIES ${LIBMODBUS_NAME})
check_symbol_exists(modbus_rtu_set_recv_filter ${LIBMODBUS_NAME}/modbus-rtu.h MODBUSPP_HAVE_RTU_MULTI_SLAVES)

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

include_directories(
  ${LIBMODBUS_INCLUDE_DIRS} 
  ${MODBUSPP_INC_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory (dev)
add_subdirectory (lib)
add_subdirectory (doc)

# CMake Package ----------------------------------------------------------------

# Add all targets to the build-tree export set
if (MODBUSPP_WITH_STATIC)
  set (exported_targets modbuspp-shared modbuspp-static)
else(MODBUSPP_WITH_STATIC)
  set (exported_targets modbuspp-shared)
endif(MODBUSPP_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/modbuspp.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE modbuspp)

# Create the modbuspp-config.cmake and modbuspp-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${MODBUSPP_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${MODBUSPP_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${MODBUSPP_INC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib")
configure_file(modbuspp-config.cmake.in
  "${PROJECT_BINARY_DIR}/modbuspp-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${MODBUSPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${MODBUSPP_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(modbuspp-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/modbuspp-config.cmake" @ONLY)

# ... for both
configure_file(modbuspp-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/modbuspp-config-version.cmake" @ONLY)

# Install the modbuspp-config.cmake and modbuspp-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/modbuspp-config.cmake"
  "${PROJECT_BINARY_DIR}/modbuspp-config-version.cmake"
  DESTINATION "${MODBUSPP_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT modbuspp DESTINATION
  "${MODBUSPP_INSTALL_CMAKE_DIR}" COMPONENT dev)

## pkg-config ------------------------------------------------------------------
foreach(item ${MODBUSPP_CFLAGS_OTHER})
  string(APPEND MODBUSPP_CFLAGS_PKGCONFIG "${item} ")
endforeach(item MODBUSPP_CFLAGS_OTHER)

foreach(item ${MODBUSPP_LDFLAGS_OTHER})
  string(APPEND MODBUSPP_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item MODBUSPP_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/modbuspp.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/modbuspp.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/modbuspp.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)

if (UNITTESTPP_FOUND AND MODBUSPP_UNIT_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory (tests)
endif()

if (MODBUSPP_WITH_DOXYGEN_DOC)
  add_dependencies(modbuspp-shared doc)
endif()

# Install the modbuspp licence files
install(FILES
  "${PROJECT_SOURCE_DIR}/COPYING.LESSER"
  DESTINATION "${MODBUSPP_INSTALL_DATA_DIR}" COMPONENT lib)

install(FILES
  "${PROJECT_SOURCE_DIR}/README.md"
  DESTINATION "${MODBUSPP_INSTALL_DOC_DIR}" COMPONENT lib)

install ( DIRECTORY "${MODBUSPP_INC_DIR}/" 
          DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev)
install ( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h 
          ${CMAKE_CURRENT_BINARY_DIR}/version.h 
          DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev)

### Debian Packages generation -------------------------------------------------
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
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libstdc++6,${LIBMODBUS_PACKAGE} (>= ${LIBMODBUS_VERSION})")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libmodbuspp-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (development files)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the development files.")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}),pkg-config,git-core,${LIBMODBUSDEV_PACKAGE} (>= ${LIBMODBUS_VERSION})")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${MODBUSPP_SRC_DIR}/doc/README-deb.md")

  set(CPACK_DEBIAN_DOC_PACKAGE_NAME "libmodbuspp-doc")
  set(CPACK_COMPONENT_DOC_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (documentation)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the API documentation.")
  set(CPACK_DEBIAN_DOC_FILE_NAME "lib${PROJECT_NAME}-doc_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  #set(CPACK_DEBIAN_DOC_PACKAGE_DEPENDS "${CPACK_DEBIAN_DEV_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_DOC_PACKAGE_SECTION "libdevel")
    
endif (CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
