# World

## Overview

`world` is a small C++17/macOS graphics prototype. It uses FreeGLUT and
legacy OpenGL immediate-mode rendering to display simple 2D shapes in a
world viewed through an orthographic camera.

The project is intentionally early-stage: all application code currently
lives in `world.cpp`; there are no automated tests or CI configuration.

## Build and run

Build the executable from the repository root:

```sh
make
./world
```

The Makefile assumes macOS, Apple frameworks, and a Homebrew installation at
`/opt/homebrew` with FreeGLUT headers and libraries available. The Visual
Studio Code task **Build world** runs `make`.

Run `make clean` to remove the compiled `world` executable.

## Current structure

- `Point<T>` and `Size<T>` are basic geometry value types.
- `Shape` is the drawable base class. `Square` and `Triangle` render red
  primitives with immediate-mode OpenGL.
- `Unit` exclusively owns its `Shape` via `std::unique_ptr` together with a
  world location.
- `World` owns a vector of `Unit` values and renders them.
- `Camera` configures the orthographic projection and is updated when the
  GLUT window is resized.

At startup, the application creates a 1024×768 window and renders a square
at `(50, 50)` and a triangle at `(200, 50)`.

## Maintenance notes

- `Unit` owns its shape. Pass shapes using `std::make_unique<ConcreteShape>`;
  units are move-only as a result.
- The program currently uses global raw pointers for `World` and `Camera` and
  does not delete them. `World::~World()` is declared but not defined. Resolve
  ownership deliberately if changing lifetime management.
- Rendering uses the deprecated fixed-function OpenGL API (`glBegin`, matrix
  stack, `glOrtho`). Keep this in mind before targeting modern OpenGL or
  platforms where compatibility OpenGL is unavailable.
- Avoid committing generated build artifacts such as `world` and `world.dSYM/`.
