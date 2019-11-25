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

# libmodbuspp CMakeLists.txt
cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB_RECURSE src_modbuspp  ${MODBUSPP_SRC_DIR}/*.cpp)

set (hdr_modbuspp 
  ${MODBUSPP_INC_DIR}/modbuspp.h
  ${MODBUSPP_INC_DIR}/modbuspp/bufferedslave.h
  ${MODBUSPP_INC_DIR}/modbuspp/data.h
  ${MODBUSPP_INC_DIR}/modbuspp/device.h
  ${MODBUSPP_INC_DIR}/modbuspp/global.h
  ${MODBUSPP_INC_DIR}/modbuspp/master.h
  ${MODBUSPP_INC_DIR}/modbuspp/message.h
  ${MODBUSPP_INC_DIR}/modbuspp/netlayer.h
  ${MODBUSPP_INC_DIR}/modbuspp/popl.h
  ${MODBUSPP_INC_DIR}/modbuspp/rtulayer.h
  ${MODBUSPP_INC_DIR}/modbuspp/server.h
  ${MODBUSPP_INC_DIR}/modbuspp/slave.h
  ${MODBUSPP_INC_DIR}/modbuspp/slavereport.h
  ${MODBUSPP_INC_DIR}/modbuspp/swap.h
  ${MODBUSPP_INC_DIR}/modbuspp/tcplayer.h
  ${MODBUSPP_INC_DIR}/modbuspp/timeout.h
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_modbuspp} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (libsrc ${src_modbuspp})

include_directories(BEFORE
  ${LIBMODBUS_INCLUDE_DIRS} 
  ${MODBUSPP_INC_DIR} 
  ${MODBUSPP_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(modbuspp-shared SHARED ${libsrc})
target_compile_definitions(modbuspp-shared PUBLIC MODBUSPP_WITH_CONFIG_H)
set_target_properties(modbuspp-shared PROPERTIES 
  OUTPUT_NAME modbuspp 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${MODBUSPP_VERSION} 
  SOVERSION ${MODBUSPP_VERSION_MAJOR}
  PUBLIC_HEADER "${hdr_public}"
  )

if(MODBUSPP_WITH_STATIC)
  add_library(modbuspp-static STATIC ${libsrc})
  target_compile_definitions(modbuspp-static PUBLIC MODBUSPP_WITH_CONFIG_H)
  set_target_properties(modbuspp-static PROPERTIES
    OUTPUT_NAME modbuspp 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${MODBUSPP_VERSION} 
  )
endif(MODBUSPP_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (MODBUSPP_WITH_STATIC)
  set (exported_targets modbuspp-shared modbuspp-static)
else(MODBUSPP_WITH_STATIC)
  set (exported_targets modbuspp-shared)
endif(MODBUSPP_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/libmodbuspp.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE libmodbuspp)

# Create the modbuspp-config.cmake and modbuspp-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${MODBUSPP_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${MODBUSPP_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${MODBUSPP_INC_DIR}" "${MODBUSPP_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/libmodbuspp")
configure_file(libmodbuspp-config.cmake.in
  "${PROJECT_BINARY_DIR}/libmodbuspp-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${MODBUSPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${MODBUSPP_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(libmodbuspp-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libmodbuspp-config.cmake" @ONLY)

# ... for both
configure_file(libmodbuspp-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/libmodbuspp-config-version.cmake" @ONLY)

# Install the libmodbuspp-config.cmake and libmodbuspp-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libmodbuspp-config.cmake"
  "${PROJECT_BINARY_DIR}/libmodbuspp-config-version.cmake"
  DESTINATION "${MODBUSPP_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT modbuspp DESTINATION
  "${MODBUSPP_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the modbuspp licence files
install(FILES
  "${PROJECT_SOURCE_DIR}/COPYING.LESSER"
  DESTINATION "${MODBUSPP_INSTALL_DATA_DIR}" COMPONENT lib)

install(FILES
  "${PROJECT_SOURCE_DIR}/README.md"
  DESTINATION "${MODBUSPP_INSTALL_DOC_DIR}" COMPONENT lib)

install (TARGETS modbuspp-shared
  # IMPORTANT: Add the modbuspp library to the "export-set"
  EXPORT modbuspp
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev
  )

if(MODBUSPP_WITH_STATIC)
  install (TARGETS modbuspp-static 
    EXPORT modbuspp
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(MODBUSPP_WITH_STATIC)


## pkg-config ------------------------------------------------------------------
foreach(item ${MODBUSPP_CFLAGS_OTHER})
  string(APPEND MODBUSPP_CFLAGS_PKGCONFIG "${item} ")
endforeach(item MODBUSPP_CFLAGS_OTHER)

foreach(item ${MODBUSPP_LDFLAGS_OTHER})
  string(APPEND MODBUSPP_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item MODBUSPP_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/libmodbuspp.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libmodbuspp.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libmodbuspp.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
