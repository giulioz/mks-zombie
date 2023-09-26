# JX-10/MKS-70/JX-8P Zombie Utility

A software tool to control the voice boards of Roland JX-10/MKS-70/JX-8P with a serial adapter, as described in my [YouTube video](https://www.youtube.com/watch?v=X7epZf2e9so).

![screenshot](docs/screenshot.png)

[![Youtube video](https://img.youtube.com/vi/X7epZf2e9so/0.jpg)](https://www.youtube.com/watch?v=X7epZf2e9so)

## Building

This project requires a C++ compiler and CMake.

```sh
git clone --recurse-submodules https://github.com/giulioz/mks-zombie.git
cd mks-zombie
mkdir build
cd build
cmake ..
make
./mks-zombie
```
