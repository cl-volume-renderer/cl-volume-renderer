# gpgpu-volume-renderer

gpgpu-volume-renderer project by Pit Henrich and Marcel Hollerbach

# Build:
Ensure that you have installed the required packages (sdl2, zlib, gl, opencl, openmp)
To install OpenCL/GL please refer to your hardware manufacturers instructions. Other packages
can be installed as follows (debian):

```
sudo apt install libsdl2-dev zlib1g-dev libomp-dev
```


To build:
```
mkdir build
cd build
meson ..
ninja
```

This will automatically download stb & imgui

Binary to launch the application: ./app/volume-renderer *nrrd-file* *hdr-environment-map*
Notice: You can also use regular jpg or png for the environment map.

# Platforms
Tested on:
* Nvidia GeForce GTX 1050 4GB (Mobile)
* Nvidia GeForce GTX 750 2GB (Desktop)
* AMD Vega 56 8GB (Desktop)
* Intel HD 4400 2GB (Mobile)
