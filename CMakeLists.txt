################################################################################
# MIT License
#
# Copyright (c) 2024 Kai Wang
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

cmake_minimum_required(VERSION 3.16)

set (TARGET_NAME easy_debug_view)

project(${TARGET_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake-predefined")

set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DUNICODE -D_UNICODE)

file(GLOB SOURCE_FILES
    "source/*.h"
    "source/*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCE_FILES})

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

install(TARGETS ${TARGET_NAME} DESTINATION .)
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION .)
install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION / OPTIONAL COMPONENT symbols)

set(CPACK_PACKAGE_FILE_NAME "easy_debug_view")
set(CPACK_PACKAGE_NAME "easy_debug_view")
set(CPACK_PACKAGE_DESCRIPTION "Easy and fast debug output viewer for Windows")

set(CPACK_GENERATOR "ZIP")

include(CPack)