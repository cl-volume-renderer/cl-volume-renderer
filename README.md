![UIImage](https://user-images.githubusercontent.com/1415748/87870529-39869a00-c9a9-11ea-9801-76596a20fa96.jpg)

# Build:
Ensure that you have installed the required packages (sdl2, zlib, gl, opencl, openmp). Also ensure you are
using a modern build environment (we used **GCC 9.3** or newer), as we rely on modern language features.
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

This will automatically download stb & imgui.

Launch application: 
```
./app/volume-renderer *nrrd-file* *hdr-environment-map*
```

**Notice:** You can also use regular JPG or PNG files for the environment map.

# Controls:
* Movement: W, A, S, D, E, Q
  * Modifier: Shift, Ctrl
* Update frame: R



# Platforms
Tested on:
* Nvidia GeForce GTX 1050 4GB (Mobile)
* Nvidia GeForce GTX 750 2GB (Desktop)
* AMD Vega 56 8GB (Desktop)
* Intel HD 4400 2GB (Mobile)
