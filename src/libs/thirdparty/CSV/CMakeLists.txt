# Define library. Only source files here!
project(libcsv VERSION 2.1.1 LANGUAGES CXX)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Define C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(csv INTERFACE)

message(STATUS "CSV Version: ${PROJECT_VERSION}")

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(csv INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# If we have compiler requirements for this library, list them
# here
target_compile_features(csv INTERFACE
    cxx_std_11)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS csv EXPORT CSVConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT CSVConfig DESTINATION share/CSV/cmake)

# This makes the project importable from the build directory
export(TARGETS csv FILE CSVConfig.cmake)