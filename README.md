# Audio Slicer

C++ implementation of OpenVPI [Audio Slicer](https://github.com/openvpi/audio-slicer). Most of the algorithms are adapted from there.

# Build from source

C++ compiler with C++17 support is required.

### Vcpkg

This project uses vcpkg to manage dependencies. Run the following command to install dependencies:

```bash
vcpkg install libsndfile argparse qt5-base
```

To build with CMake, you need to set `CMAKE_TOOLCHAIN_FILE` variable:

```bash
-DCMAKE_TOOLCHAIN_FILE=${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake
```

Please change  `${VCPKG_PATH}` to the location of vcpkg installation. CMake will find all the packages needed.

### Build options

You can set `AUDIO_SLICER_CLI` and `AUDIO_SLICER_GUI` options to choose which versions (CLI / GUI) to build.

```bash
-DAUDIO_SLICER_CLI=ON -DAUDIO_SLICER_GUI=ON
```

For macOS build, you can turn on `BUILD_MACOSX_BUNDLE` option to build macOS app bundles.

## Open-source softwares used

* [libsndfile](https://github.com/libsndfile/libsndfile)
  * GNU LGPL v2.1
* [vcpkg](https://github.com/microsoft/vcpkg)
  * MIT License
* [argparse](https://github.com/p-ranav/argparse)
  * MIT License
* [Qt 5](https://www.qt.io/)
  * GNU LGPL v3


