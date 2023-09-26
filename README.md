# JX-10/MKS-70/JX-8P Zombie Utility

A software tool to control the voice boards of Roland JX-10/MKS-70/JX-8P with a serial adapter, as described in my [YouTube video](https://www.youtube.com/watch?v=X7epZf2e9so).

![screenshot](docs/screenshot.png)

<iframe width="560" height="315" src="https://www.youtube.com/embed/X7epZf2e9so?si=ebBpNKQuTE-0qziG" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

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
