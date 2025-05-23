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

cmake_minimum_required(VERSION 3.0)

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
    ${PROJECT_SOURCE_DIR}/include/modbuspp/bufferedslave.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/data.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/device.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/global.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/master.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/message.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/netlayer.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/request.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/response.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/router.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/rtulayer.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/server.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/slave.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/slavereport.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/swap.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/tcplayer.h
    ${PROJECT_SOURCE_DIR}/include/modbuspp/timeout.h
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
  
  #message("MODBUSPP_INSTALL_DOC_DIR=${MODBUSPP_INSTALL_DOC_DIR}")
  #message("INSTALL_DATA_DIR=${INSTALL_DATA_DIR}")

  # API documentation
  install(DIRECTORY ${CMAKE_BINARY_DIR}/html/
          DESTINATION ${MODBUSPP_INSTALL_DOC_DIR}/api-manual COMPONENT doc)
  
  # manpages
  install(DIRECTORY ${CMAKE_BINARY_DIR}/man/
          DESTINATION ${INSTALL_DATA_DIR}/man 
          COMPONENT doc
          FILES_MATCHING PATTERN "Modbus_*.3")

  # Examples
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/
          DESTINATION ${MODBUSPP_INSTALL_EXAMPLES_DIR}
          COMPONENT doc
          FILES_MATCHING PATTERN "*.c" PATTERN "*.h" PATTERN "*.cpp" PATTERN "*.mk" PATTERN "*.txt" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt" PATTERN "README*"
          PATTERN ".clang/" EXCLUDE
          PATTERN "Debug/" EXCLUDE
          PATTERN "Release/" EXCLUDE
          PATTERN ".codelite/" EXCLUDE
          PATTERN "cmake-build*/" EXCLUDE)

endif(MODBUSPP_WITH_DOXYGEN_DOC)
