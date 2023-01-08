# Audio Slicer

C++ implementation of OpenVPI [Audio Slicer](https://github.com/openvpi/audio-slicer). Most of the algorithms are adapted from there.

# Build from source

This project uses vcpkg to manage dependencies. Run the following command to install dependencies:

```bash
vcpkg install libsndfile argparse
```

To build with CMake, you need to set `CMAKE_TOOLCHAIN_FILE` variable:

```
-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

The project requires C++17.

## Open-source softwares used

* [libsndfile](https://github.com/libsndfile/libsndfile)
  * GNU LGPL v2.1
* [vcpkg](https://github.com/microsoft/vcpkg)
  * MIT License
* [argparse](https://github.com/p-ranav/argparse)
  * MIT License

