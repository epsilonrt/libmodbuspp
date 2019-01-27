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

# doc CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

set (MODBUSPP_WITH_DOXYGEN_DOC 0 CACHE BOOL "Enable build of the documentation")

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

if("${CMAKE_PROJECT_NAME}" STREQUAL "Project")
  set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

if (MODBUSPP_WITH_DOXYGEN_DOC)

  find_package(Doxygen REQUIRED)
  set(DOXYGEN_README ${PROJECT_SOURCE_DIR}/README.md)
  set(DOXYGEN_FILE ${CMAKE_BINARY_DIR}/Doxyfile)
  set(DOXYGEN_MAINPAGE ${CMAKE_BINARY_DIR}/main_page.dox)

  set(DOXYGEN_INPUT_LIST 
    ${PROJECT_SOURCE_DIR}/include/modbuspp.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp-data.h
    )

  set(DOXYGEN_INPUT "${CMAKE_BINARY_DIR}/main_page.dox")
  foreach(item ${DOXYGEN_INPUT_LIST})
    # get_filename_component(item_dir ${item} DIRECTORY)
    string(APPEND DOXYGEN_INPUT " ${item} ")
  endforeach(item ${DOXYGEN_INPUT_LIST})

  #message("DOXYGEN_INPUT=${DOXYGEN_INPUT}")

  configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
  configure_file(build_main_page.sh.in ${CMAKE_BINARY_DIR}/build_main_page.sh @ONLY)

  add_custom_target(main_page.dox
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation Main Page..."
    COMMAND sh ${CMAKE_BINARY_DIR}/build_main_page.sh > /dev/null
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    DEPENDS main_page_header.dox main_page_footer.dox ${PROJECT_SOURCE_DIR}/README.md
    )

  add_custom_target(html
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE} > /dev/null
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    DEPENDS main_page.dox DoxygenLayout.xml Doxyfile.in ${DOXYGEN_INPUT_LIST}
    )
  
  add_custom_target(doc)
  add_dependencies(doc main_page.dox html)
  
  file(GLOB manpages  ${CMAKE_BINARY_DIR}/man/man3/Modbus_*.3)
  #message("MODBUSPP_INSTALL_DOC_DIR=${MODBUSPP_INSTALL_DOC_DIR}")
  #message("INSTALL_DATA_DIR=${INSTALL_DATA_DIR}")

  install(DIRECTORY ${CMAKE_BINARY_DIR}/html/
          DESTINATION ${MODBUSPP_INSTALL_DOC_DIR}/api-manual COMPONENT doc)
  install(FILES ${manpages}
          DESTINATION "${INSTALL_DATA_DIR}/man/man3" COMPONENT doc)

endif(MODBUSPP_WITH_DOXYGEN_DOC)
