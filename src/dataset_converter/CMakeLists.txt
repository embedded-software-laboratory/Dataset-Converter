# Enable Qt5 cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (APPLE)
    set(PLATFORM_SOURCE src/platform/OSXHideTitleBar.mm)
    set(FRAMEWORK_RES ${CMAKE_SOURCE_DIR}/frameworks/logo.icns)
endif ()

# Define an executable
add_executable(cpm_dataset_converter ${PLATFORM_SOURCE} ${FRAMEWORK_RES}
        resources/resources.qrc
        src/main.cpp
        src/MainWindow.cpp
        src/worker/DatasetParser.cpp
        src/worker/LaneletHandler.cpp
        src/worker/ScenarioHandler.cpp
        src/dialog/LoadDatasetDialog.cpp
        src/dialog/SaveScenarioDialog.cpp
        src/dialog/LoadScenarioDialog.cpp
        src/visualisation/LaneletVisualisation.cpp
        src/visualisation/ScenarioVisualization.cpp
        src/visualisation/GraphicsViewZoomHandler.cpp
        src/visualisation/GraphicsViewClickHandler.cpp
        src/visualisation/graphics_items/NodeItem.cpp
        src/visualisation/graphics_items/LaneletItem.cpp
        src/visualisation/graphics_items/WayItem.cpp
        src/visualisation/graphics_items/ExtendedObjectItem.cpp)

target_include_directories(cpm_dataset_converter PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(cpm_dataset_converter
        Qt5::Widgets
        Qt5::Core
        Qt5::Svg
        lanelet2_core
        lanelet2_projection
        lanelet2_io
        dataset_converter)

if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework ServiceManagement -framework Foundation -framework Cocoa")
    # Setting application meta information
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT_NAME}")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - Version ${VERSION}")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "Informatik 11 - Embedded Software")
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright © 2021 Simon Schäfer. All rights reserved.")
    #
    set_source_files_properties(${FRAMEWORK_RES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/)
    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_ICON_FILE "logo.icns")

    set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/frameworks/MacOSXBundleInfo.plist.in")
elseif (UNIX)
    # Fix double click bug
    set(CMAKE_EXE_LINKER_FLAGS -no-pie)

    # Setting application meta information
    set(PROJECT_PATH "${CMAKE_BINARY_DIR}/cpm_dataset_converter")
    set(PROJECT_ICON "${CMAKE_BINARY_DIR}/${PROJECT_NAME_LOWER}-logo.png")

    # Copy launcher files
    file(COPY ${CMAKE_SOURCE_DIR}/frameworks/${PROJECT_NAME_LOWER}-logo.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    configure_file(${CMAKE_SOURCE_DIR}/frameworks/${PROJECT_NAME_LOWER}.desktop.in ${PROJECT_NAME_LOWER}-launcher.desktop)
endif ()