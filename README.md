# CPM Dataset Converter

The Datasetconverter is a Qt5 based GUI application with two main functions.
It allows to convert different datasets into CPM scenarios (a format based on the CommonRoad format) and to create or edit Lanelet2 maps for the CPM Lab.
The different data sets are converted on the fly into the CPM scenario format and visualised.
The user can then select a section of a recording to export a scenario. The data sets themselves must be downloaded separately from the various providers.

![Screenshot Ubuntu 22.04](documentation/screenshot-ubuntu.png)
<div align="center">Dataset Converter in Ubuntu 22.04</div>

![Screenshot Ubuntu 22.04](documentation/screenshot-macos.png)
<div align="center">Dataset Converter in MacOs <br> Scenario Loaded from the rounD dataset</div>

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

## Acknowledgements
We acknowledge the financial support for this project by the Exploratory Teaching Space of the RWTH Aachen University (Germany).

## References
* [1] A. Mokhtarian, S. Schäfer, B. Alrifaee. "CPM Olympics: Development of Scenarios for Benchmarking in
Networked and Autonomous Driving". In 33rd IEEE Intelligent Vehicles Symposium (IV) 2022, accepted.
