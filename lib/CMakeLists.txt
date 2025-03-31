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
cmake_minimum_required(VERSION 3.0)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

  
file(GLOB_RECURSE src_modbuspp  ${MODBUSPP_SRC_DIR}/*.cpp)
set (libsrc ${src_modbuspp})

# this is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})
get_target_property(JSON_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(objlib BEFORE PRIVATE
  ${LIBMODBUS_INCLUDE_DIRS} 
  ${MODBUSPP_INC_DIR} 
  ${MODBUSPP_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JSON_INCLUDE_DIRS}
)

# shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(modbuspp-shared SHARED $<TARGET_OBJECTS:objlib>)
target_link_libraries(modbuspp-shared PRIVATE nlohmann_json::nlohmann_json)
set_target_properties(modbuspp-shared PROPERTIES 
  OUTPUT_NAME modbuspp 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${MODBUSPP_VERSION} 
  SOVERSION ${MODBUSPP_VERSION_MAJOR}
  )

if(MODBUSPP_WITH_STATIC)
  add_library(modbuspp-static STATIC $<TARGET_OBJECTS:objlib>)
  target_link_libraries(modbuspp-static PRIVATE nlohmann_json::nlohmann_json)
  set_target_properties(modbuspp-static PROPERTIES
    OUTPUT_NAME modbuspp 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${MODBUSPP_VERSION} 
  )
endif(MODBUSPP_WITH_STATIC)

install (TARGETS modbuspp-shared
  # IMPORTANT: Add the modbuspp library to the "export-set"
  EXPORT modbuspp
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )

if(MODBUSPP_WITH_STATIC)
  install (TARGETS modbuspp-static 
    EXPORT modbuspp
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev
  )
endif(MODBUSPP_WITH_STATIC)
