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

# modbuspp tests CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

include(SubDirList)
SUBDIRLIST(TESTS ${CMAKE_CURRENT_SOURCE_DIR})

find_package(modbuspp CONFIG)

add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND} -V)
link_directories(${MODBUSPP_LIBRARY_DIRS})

foreach(test ${TESTS})
  message (STATUS " add test ${test}")
  # create an executable, which instantiates a runner from UnitTest++
  add_executable(${test} ${test}/main.cpp)
  add_dependencies(${test} modbuspp-shared)

  # depending on the framework, you need to link to it
  target_link_libraries(${test} ${MODBUSPP_LIBRARIES} ${UNITTESTPP_LIBRARIES})
  target_compile_options(${test} PUBLIC ${MODBUSPP_CFLAGS} ${UNITTESTPP_CFLAGS})

  # now register the executable with CTest
  add_test(NAME ${test} COMMAND ${test})
endforeach()

add_dependencies(build_and_test modbuspp-shared)
