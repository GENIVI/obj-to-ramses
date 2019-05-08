#  -------------------------------------------------------------------------
#  Copyright (C) 2018 BMW AG
#  -------------------------------------------------------------------------
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.
#  -------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

project (obj2ramses)

set (obj2ramses_VERSION_MAJOR 0)
set (obj2ramses_VERSION_MINOR 1)

add_subdirectory(external)

# TODO after first version is done, refactor project structure
file(GLOB source_files
    src/*.cpp
    src/include/*.h
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(RND_PLATFORM platform-windows-wgl-4-2-core)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # TODO add support for other backends on Linux, e.g. Wayland
    set(RND_PLATFORM platform-x11-egl-es-3-0)
endif()

set(ramses_dependencies
    ramses-client-api
    ramses-client
    ramses-renderer-api
    ramses-renderer-lib
    ${RND_PLATFORM}
    ramses-framework-api
    ramses-framework)

add_executable(obj2ramses ${source_files})
target_link_libraries(obj2ramses ${ramses_dependencies})
target_include_directories(obj2ramses PRIVATE src/include)

add_definitions(-DRAMSES_LINK_STATIC)

# Collect asset files
file(GLOB_RECURSE ASSETS
    LIST_DIRECTORIES FALSE
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/res/*
)

# Copy asset files to build folder
foreach(asset ${ASSETS})
  file(RELATIVE_PATH relativePath ${PROJECT_SOURCE_DIR} ${asset})
  configure_file(${relativePath} ${CMAKE_CURRENT_BINARY_DIR}/${relativePath} COPYONLY)
endforeach()
