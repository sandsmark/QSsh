cmake_minimum_required(VERSION 3.14)

# set the project name and version
project(QSsh VERSION 1.0 LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4005")
    set(BOTAN_LIB ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/botan.lib)
else(MSVC)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(Botan REQUIRED IMPORTED_TARGET GLOBAL botan-2)

    set_property(GLOBAL PROPERTY BOTAN_LIB PkgConfig::Botan)
endif(MSVC)

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

add_subdirectory(src)
add_subdirectory(examples)

# INSTALL RULES
install(EXPORT QSsh-targets DESTINATION lib)
install(EXPORT QSsh-targets
  FILE QSshTargets.cmake
  NAMESPACE QSsh::
  DESTINATION share/QSsh/cmake
)


install(FILES "${CMAKE_SOURCE_DIR}/cmakeFindModules/QSshConfig.cmake"
        DESTINATION share/QSsh/cmake)
