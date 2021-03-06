# Define library. Only source files here!
project(dataset_converter_common VERSION 0.1 LANGUAGES CXX)

add_library(dataset_converter_common
        src/inD/InDScenario.cpp
        src/inD/InDParser.cpp
        src/roundD/RounDScenario.cpp
        src/roundD/RounDParser.cpp
        #src/KoPer/KoPerParser.cpp
        #src/KoPer/KoPerScenario.cpp
        src/DUT/DutParser.cpp
        src/DUT/DutScenario.cpp
        src/DatasetParser.cpp
        src/DatasetScenario.cpp)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(dataset_converter_common PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# If we have compiler requirements for this library, list them
# here
target_compile_features(dataset_converter_common
        PUBLIC cxx_auto_type
        PRIVATE cxx_variadic_templates)

# Depend on a library that we defined in the top-level file
target_link_libraries(dataset_converter_common
        csv
        cpm_scenario
        Eigen3::Eigen)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS dataset_converter_common EXPORT DatasetConverterCommonConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT DatasetConverterCommonConfig DESTINATION share/DatasetConverterCommon/cmake)

# This makes the project importable from the build directory
export(TARGETS dataset_converter_common FILE DatasetConverterCommonConfig.cmake)
