# My Smart Study Bedroom — Phase 1 Lite Demo

A simplified initial progress demonstration for the teacher, using the existing **C++17, OpenGL 3.3 Core Profile, GLFW, GLAD and GLM** project. The room keeps basic models, camera navigation and three shading modes. Advanced lighting and animation are deferred.

![Phase 1 Lite Demo in Phong shading](docs/screenshots/Phong.png)

## Build and run

From the project folder in VS Code's PowerShell terminal:

```powershell
.\build.bat
.\run.bat
```

You can also double-click these files. Once built, `run.bat` starts the current demo. The existing MSYS2 UCRT64 GCC/CMake/Ninja toolchain is used, and dependencies are stored locally in `external/`.

Equivalent CMake commands:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
.\build\smart_study_bedroom.exe
```

Keep the executable and its sibling `shaders/` folder together when copying the application. Building requires a C/C++17 compiler, CMake 3.20+, Ninja and an OpenGL 3.3-capable driver; no internet or Python is needed. Windows is the tested platform.

## Controls (unchanged)

Click the window to focus it. Press **Tab** to enable mouse look.

| Input | Action |
| --- | --- |
| W / A / S / D | Move |
| Q / E | Down / up |
| Tab + mouse | Capture/release pointer and look around |
| Mouse wheel | Zoom |
| **1 / 2 / 3** | **Flat / Gouraud / Phong** |
| V | Interior / whole-room overview |
| R | Reset camera |
| F12 | Save a BMP under `screenshots/` in the working directory |
| Escape | Exit |

There are no lighting, lamp or object-interaction controls. The overview hides the front wall, ceiling and attached ceiling fixtures. The camera has no collision.

## Current scene

- Room with a plain floor, walls, ceiling, window/door openings and simple trim.
- Bed with a mattress, one curved pillow and plain blanket; bedside table and a three-part lamp model.
- Wardrobe with two simple panels and handles.
- Tabletop with four legs; chair with a seat, backrest and four legs.
- Laptop base and screen; bookshelf with three plain books.
- Window, two flat curtain panels, static clock, **one** wall frame and rectangular rug.
- Closed door, stationary ceiling fan and static ceiling-light model.

**The plant is permanently removed and must not return.** Dustbin, study lamp, extra frame, shelf ornament, desk accessories, detailed keyboard, draped fabric and rug pattern are removed for this demo. Those non-plant details may be extended later if requested.

## Shading and fixed lighting

One fixed point light and an ambient term provide a basic ambient + diffuse + specular demonstration. There are no spotlights, attenuation calculations, toggles or dynamic light controls. The bedside lamp is geometry only; it does not emit light. The blue window backdrop uses the existing unlit shader.

- **Flat:** one face normal and one completed color per triangle, without color interpolation.
- **Gouraud:** calculate lighting at vertices, then interpolate their colors.
- **Phong:** interpolate position/normal and calculate lighting per fragment.

Use the pillow or lamp to explain the differences. All three shaders share the same light/material settings. Normal transformation still uses the inverse transpose for scaled shapes. Phong remains the default.

## Simple code tour

Start at `src/Scene.cpp`, which places objects, then open `src/objects/Study.cpp`. `drawTable` draws one top and four legs; `drawLaptop` draws only a base and a screen.

The existing Shader, Camera, Mesh and DrawContext structure is unchanged. `Primitives.*` now contains only five shared shapes: box, rounded box, sphere, cylinder and frustum. Meshes upload their smooth and flat variants once. No new architecture or dependencies were added.

See [PROJECT_PLAN.md](PROJECT_PLAN.md) for current scope, completed work and future TODOs. The richer files were staged before this refactor; staging was left untouched. At inspection there were no commits, so this is not yet a committed history checkpoint.

## Verification and views

```powershell
.\build\smart_study_bedroom.exe --verify
.\build\smart_study_bedroom.exe --view overview
.\build\smart_study_bedroom.exe --view bedroom --mode flat
.\build\smart_study_bedroom.exe --view study --capture docs/screenshots/study.bmp
```

The existing verification checks shader output, camera handlers, resize, unchanged mesh uploads, OpenGL errors and a 30-second static-scene comparison. It saves BMPs using a hidden real OpenGL window. See [verification results](docs/VERIFICATION.md).

Current screenshots: [Flat](docs/screenshots/Flat.png), [Gouraud](docs/screenshots/Gouraud.png), [Phong](docs/screenshots/Phong.png), [overview](docs/screenshots/overview.png), [bedroom](docs/screenshots/bedroom.png), [study](docs/screenshots/study.png).

## Later full version

Short source TODOs cover optional richer bedding/books/desk details, curtain folds, door/fan motion and additional lighting. Clock animation, advanced interaction, weather and a day/night cycle also remain deferred until explicitly requested. Plant restoration is permanently excluded.
