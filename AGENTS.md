# rgd — OpenGL / graphics programming learning project

## Build & run
```sh
cmake -B build                 # configure (uses PkgConfig for glfw3)
cmake --build build            # compile
cmake --build build --target run    # run from project root (CWD=.)
cmake --build build --target debug  # run under gdb
```

A `build/` directory with `compile_commands.json` already exists.

## Dependencies
- Nix: `nix-shell shell.nix` (gcc, cmake, glfw3, spdlog, gdb, clang-tools, libGL, X11 libs)
- Otherwise: cmake, gcc/clang, pkg-config, glfw3, spdlog

## Architecture
- **Entrypoint**: `src/main.cpp` — 1720×890 GL 3.3 core window, GLFW+GLAD
- **GL error checking**: `GLCALL(glXxx(...))` macro (`src/rgd.h`) wraps every GL call
- **Logger**: `LOG_INFO/WARN/DEBUG/FATAL(...)` macros wrapping spdlog (`src/logger.h`)
- **Shader loading**: `ResourceManager::LoadShader("shaders/<name>/vertex.glsl", ...)`
- **ImGui**: vendored in `include/imgui/`, GLFW+OpenGL3 backends bundled
- **GLM/GLAD/STB**: all vendored in `include/`

## Known issues (missing files — will crash at startup)
- `shaders/specular_map/` — referenced in main.cpp:106-107 but does not exist
- `res/planks.png`, `res/planksSpec.png` — referenced in main.cpp:132-135 but do not exist

## Controls
| Key | Action |
|-----|--------|
| WASD | Move camera |
| Mouse | Look around |
| ESC | Quit |
| T | Toggle ImGui demo window |
| TAB | Toggle debug drawing |

## Style
- C++20, `.clang-format`: LLVM base, 4-space indent, Allman braces, `PointerAlignment: Left`
- `SortIncludes: false`, `IncludeBlocks: Preserve` — do not reorder includes
- No linter script — run `clang-format -i src/*.cpp src/*.h` manually
