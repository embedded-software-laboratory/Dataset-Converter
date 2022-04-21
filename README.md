# CPM Dataset Converter

<img align="right" src="documentation/logo.png" width="128" alt="Logo">

The Datasetconverter is a Qt5 based GUI application with two main functions.
It allows to convert different datasets into CPM scenarios (a format based on the CommonRoad format) and to create or edit Lanelet2 maps for the CPM Lab.
The different data sets are converted on the fly into the CPM scenario format and visualised.
The user can then select a section of a recording to export a scenario. The data sets themselves must be downloaded separately from the various providers.

## Clone

The tool is built with submodules by default.
It is possible to build without submodules if the cpm lanelet2 library or the cpm scenario library is installed system-wide.
Make sure to init and update the submodules or use recursive clone.

```bash
git clone git@git.rwth-aachen.de:CPM/Project/Lab/tools/dataset-converter.git --recursive
```

```bash
git submodule init
git submodule update
```

## Build

This tool is built with the common CMake command line interface of simillar tools.

```bash
cd <dir>
mkdir build
cd build
cmake ..
cmake --build .
```

The tool supports installation via the CMake command line interface of simillar tools.

```bash
cd <dir>
mkdir build
cd build
cmake .. --CMAKE_INSTALL_PREFIX="<install-dir>"
cmake --build . --target install
```
