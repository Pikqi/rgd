### Graphics project for a university course

This project showcases some of the graphics techinques that i have learned over the semester such as:

- Random terrain generation - Generating verticies on the CPU using layerd Perlin noise.
- Proceduraly generated clouds - Rendered via volumetric ray marching
- Water rendering - Rendering a 2d plane as water, but offseting normals to create waving effect. Water also has refraction, distorting the rendered terrain below.
- Post processing effects - For now just 2 effects, setting of exposure and a vignette. The postprocessing pipeline allows multiple effects to be layerd and allows for inspection of framebuffers between all effects in the ImGui window.

### Building instructions

Building and running has only been confirmed on a linux machine for now, but since there is no OS specific code used, it should be able to be built and run on other systems with appropriate dependencies installed.

Make sure to clone the repo with `--recursive` flag to get all submodules.

This project is using CMake and gcc, and builds most of the dependencies from submodules. Only runtime dep is OpenGL.

There is also a shell.nix file, if you are on a system with nix installed you can run nix-shell . in to get put into a shell with all build and runtime deps on a linux host.

There are some staticly included libraries such as:

- GLAD - For GL loading
- GLFW - For windowing
- ImGui - for immediate mode ui
- glm - gl maths
- stb_perlin - perlin noise implementation
- spdlog - for logging

To build and run the project (with Make) (Replace $(nproc) with number of threads you have on windows)

```
mkdir build
cd build
cmake ..

make -j$(nproc) run
```

Or with ninja:

```
mkdir build
cd build
cmake .. -G Ninja

ninja run
```

### Runtime parameters

A lot of the rendering parameters can be changed during runtime, inside of the ImGui dropdown menu.

There are also some keybinds available:

- R to recompile shaders
- 1 to toggle clouds
- 2 to toggle water
- 3 to toggle terrain
- RMB to toggle camera control
- Scroll wheel to change camera speed
- Escape to exit
