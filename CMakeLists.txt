cmake_minimum_required(VERSION 3.14)

# set the project name and version
project(QSsh VERSION 1.0 LANGUAGES CXX)

# use GNU installation dirs
include(GNUInstallDirs)

# use package configuration helpers
include(CMakePackageConfigHelpers)

# cached variables
set(QSSH_BUILD_EXAMPLES True CACHE BOOL "Build the QSsh examples.")
set(BUILD_SHARED_LIBS False CACHE BOOL "Build QSsh using shared libraries.")
set(BOTAN_INCLUDE_DIR "" CACHE PATH "Path of the Botan include directory. Set this if user specified BOTAN_LIBRARY location is used.")
set(BOTAN_LIBRARY "" CACHE FILEPATH "Path of the Botan library. Leave blank if you want to use the automated package search.")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# user specified Botan library?
# note in this case user must take care of installation of library/header files if required
if(NOT "${BOTAN_LIBRARY}" STREQUAL "")
  add_library(Botan2 UNKNOWN IMPORTED)
  if(WIN32)
    set_property(TARGET Botan2 PROPERTY IMPORTED_IMPLIB "${BOTAN_LIBRARY}")
    set_property(TARGET Botan2 PROPERTY IMPORTED_LOCATION "${BOTAN_LIBRARY}") # the location should be the .dll, but we don't care about its location. However CMake requires IMPORTED_LOCATED to be set to 'something'
  else()
    set_property(TARGET Botan2 PROPERTY IMPORTED_LOCATION "${BOTAN_LIBRARY}")
  endif()
  target_include_directories(Botan2 INTERFACE "${BOTAN_INCLUDE_DIR}")
  set_property(GLOBAL PROPERTY BOTAN_LIB Botan2)
else()
  # use package search to find the Botan module
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4005")
    set(BOTAN_LIB ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/botan.lib)
  else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(Botan REQUIRED IMPORTED_TARGET GLOBAL botan-2)
    set_property(GLOBAL PROPERTY BOTAN_LIB PkgConfig::Botan)
  endif()
endif()


find_package(QT NAMES Qt5 Qt6 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network REQUIRED)

message(STATUS "Found Qt version ${QT_VERSION_MAJOR}")

# qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# the main library
add_subdirectory(src)

# the examples
if(QSSH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# install rules
install(EXPORT QSsh-targets
  FILE QSshTargets.cmake
  NAMESPACE QSsh::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake
)

# create package configuration files
configure_package_config_file(QSshConfig.cmake.in
  "${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake/QSshConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake
  )
write_basic_package_version_file("${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake/QSshConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY ExactVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake/QSshConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake/QSshConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/QSsh/cmake)
