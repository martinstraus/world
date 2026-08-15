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

## Configuration

`world.conf` controls the ambient brightness of units and the background RGB
color. Set `ambient_light` to a value from `0.0` (dark) through `1.0` (fully
bright), and set `background_color` with three values in the same range:

```ini
ambient_light = 0.25
background_color = 0.05 0.10 0.20
```

If `world.conf` is missing or a setting is malformed or outside that range,
the application uses defaults: ambient intensity `0.25` and a dark-blue
background of `0.05 0.10 0.20`.

## Current structure

- `Point<T>` and `Size<T>` are basic geometry value types.
- `Shape` is the drawable base class. `Square`, `Triangle`, and `Circle`
  render red primitives with immediate-mode OpenGL. `Circle` is a filled
  triangle fan with 48 segments by default.
- `Unit` exclusively owns its `Shape` via `std::unique_ptr` together with a
  world location, destination, and movement speed (world units per second).
- `World` owns a vector of `Unit` values, renders them, and owns the
  configurable background color used to clear the window. It also owns an
  ambient-light intensity and a collection of radial point lights.
- `Camera` configures the orthographic projection and is updated when the
  GLUT window is resized. Hold and drag with the middle mouse button to pan
  the world. The mouse wheel zooms around the camera centre, limited to a
  range of 0.1× through 10×. Camera position and zoom interpolate smoothly
  toward input targets. Press `C` to recenter the camera on the world while
  preserving its zoom level. Left-click a unit to select it; Shift-click to
  add or remove units from the selection. Drag with the left mouse button to
  select all unit centers inside a green selection rectangle; Shift-drag adds
  that group to the current selection. Right-click sends the selected group to
  unique slots in a centered grid formation. Grid spacing encloses the largest
  selected unit, preventing destination overlap. Units animate toward their
  destinations at their own configured speeds. Press `S` to stop all selected
  units at their current positions or `Escape` to clear the selection. A
  selected unit has a translucent green selection ring rendered beneath it.

At startup, the application creates a 1024×768 window and renders a square
at `(50, 50)`, a triangle at `(200, 50)`, and a circle at `(350, 100)`.
The sample world uses a dark-blue background.
It applies ambient intensity `0.25` and includes two colorless point lights.

## Maintenance notes

- `Unit` owns its shape. Pass shapes using `std::make_unique<ConcreteShape>`;
  units are move-only as a result.
- The program currently uses global raw pointers for `World` and `Camera` and
  does not delete them. `World::~World()` is declared but not defined. Resolve
  ownership deliberately if changing lifetime management.
- Rendering uses the deprecated fixed-function OpenGL API (`glBegin`, matrix
  stack, `glOrtho`). Keep this in mind before targeting modern OpenGL or
  platforms where compatibility OpenGL is unavailable.
- Lighting does not render visible light geometry. Each unit's brightness is
  its ambient intensity plus the linearly falling-off contribution of nearby
  colorless point lights. It does not model shadows or occlusion.
- Avoid committing generated build artifacts such as `world` and `world.dSYM/`.
