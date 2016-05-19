cmake_minimum_required(VERSION 3.1)

project(vtkVRUI)

# Use c++11:
set(CMAKE_CXX_STANDARD 11)

# Find VRUI:
include(FindPkgConfig)
if(NOT VRUI_PKGCONFIG_DIR)
  find_path(VRUI_PKGCONFIG_DIR Vrui.pc)
endif()

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${VRUI_PKGCONFIG_DIR}")

# Have build directory match install dir layout:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${vtkVRUI_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${vtkVRUI_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${vtkVRUI_BINARY_DIR}/lib")

pkg_search_module(VRUI REQUIRED Vrui>=3000000)
string(REPLACE "-framework;" "-framework " VRUI_LDFLAGS "${VRUI_LDFLAGS}")
include_directories(${VRUI_INCLUDE_DIRS})
add_definitions(${VRUI_CFLAGS_OTHER})

# Find VTK:
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(VTK_VERSION VERSION_LESS "6.2")
  message(FATAL_ERROR "Require VTK version 6.2 or higher")
endif()

if(${VTK_RENDERING_BACKEND} STREQUAL "OpenGL")
  find_package(GLEW REQUIRED)
  if(NOT GLEW_FOUND)
    message(FATAL_ERROR "Glew required. Please set GLEW_DIR")
  endif()
  include_directories(${GLEW_INCLUDE_DIR})
endif()

set(sources
  vvApplication.cpp
  vvApplicationState.cpp
  vvAsyncGLObject.cpp
  vvContextState.cpp
  vvFramerate.cpp
  vvGLObject.cpp
  vvLODAsyncGLObject.cpp
  vvProgressCookie.cpp
  vvProgress.cpp
)

add_library(vtkVRUI STATIC ${sources})

target_link_libraries(vtkVRUI ${VTK_LIBRARIES} "${VRUI_LDFLAGS}")

if(${VTK_RENDERING_BACKEND} STREQUAL "OpenGL")
  target_link_libraries(vtkVRUI ${GLEW_LIBRARY})
endif()

# Install libraries
install(TARGETS vtkVRUI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY "${vtkVRUI_SOURCE_DIR}"
  DESTINATION include/vtkVRUI
  FILES_MATCHING PATTERN "*.h"
)
