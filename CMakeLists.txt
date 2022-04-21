# At LEAST 2.8 but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(CpmDatasetConverter VERSION 0.1 LANGUAGES CXX)
message("Converter Version: ${PROJECT_VERSION}")

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Define C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make sure that for homebrew users qt5 is used instead of 6
if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5/")
endif ()

# Include Qt5 as an imported target
find_package(Qt5 COMPONENTS Widgets Svg REQUIRED)
message(STATUS "Qt5 Version: ${Qt5_VERSION}")
message(STATUS "Qt5 Directory: ${Qt5_DIR}")

# Include Eigen as an imported target
find_package(Eigen3 REQUIRED NO_MODULE)
message(STATUS "Eigen3 Version: ${EIGEN3_VERSION_STRING}")

# Include Eigen as an imported target
find_package(PugiXml REQUIRED NO_MODULE)
message(STATUS "PugiXml Version: ${PugiXml_VERSION}")

# Targets that we develop here
add_subdirectory(src)
