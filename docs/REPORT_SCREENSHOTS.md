# Academic report screenshot package

All **23 required screenshots** and **six additional screenshots** are available as lossless **1920x1080 PNGs**, with the existing **4x MSAA**, depth testing and gamma/shader pipeline. No required feature was missing or skipped. The archive is `Graphics_Project_Report_Screenshots.zip` at the project root and contains exactly 29 PNGs under `report_screenshots/`; source code, binaries, logs and the CSV manifest are excluded.

## Reproduce the package

From PowerShell in the project directory:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\capture-report.ps1
```

The script builds, runs native capture, validates image decoding and comparison metadata, packages the files and checks every ZIP entry against its source SHA-256. `-SkipBuild` skips compilation; `-PackageOnly` validates/packages existing images. The complete native capture command alone is:

```powershell
.\build\smart_study_bedroom.exe --capture-report
```

The native command uses a hidden 1920x1080 OpenGL window and writes to `report_screenshots/` relative to the working directory. It exits cleanly after capture. Scene/capture options such as `--night`, `--verify` and `--capture` cannot override report states. `--shaders` can still specify the shader location. No desktop, terminal or pointer is captured.

## Image manifest

Resolution for **every row: 1920x1080**. Sizes below are bytes from the verified 2026-10-06 capture.

| Filename | Bytes | Subject / actual state |
| --- | ---: | --- |
| 01_overview.png | 789269 | Normal room perspective, day, Phong, all lamps |
| 02_birds_eye.png | 486914 | Complete layout through existing cutaway overview |
| 03_day.png | 612410 | Day, clear, curtains open, artificial lamps off |
| 04_night.png | 640724 | Same camera as day; clear night, bedside and study lamps |
| 05_rain.png | 375731 | Rain at dusk, window/curtains/skyline and desk context |
| 06_lights_off.png | 163380 | Night, all artificial lamps off |
| 07_ceiling_light.png | 792576 | Same comparison scene, ceiling lamp only |
| 08_bedside_light.png | 582327 | Same comparison scene, bedside lamp only |
| 09_study_light.png | 238671 | Same comparison scene, study spotlight only |
| 10_all_lights.png | 831989 | Same comparison scene, all three lamps |
| 11_flat.png | 411226 | Flat, fixed curved-pillow/lamp scene |
| 12_gouraud.png | 736907 | Identical scene, Gouraud only |
| 13_phong.png | 718115 | Identical scene, Phong only |
| 14_fan.png | 503337 | Actual Medium rotation: 240 degrees/second |
| 15_fan_speed.png | 501814 | Actual Max: 480 degrees/second; existing HUD/help shows MAX |
| 16_door_open.png | 360891 | Door fully open at its production 100-degree target |
| 17_curtain_open.png | 473473 | Open curtains, sunlight, normal sway |
| 18_wardrobe_open.png | 594179 | Paired doors fully open at 105 degrees; shelves/bedding |
| 19_drawer_open.png | 738455 | Desk drawer at full extension with contents visible |
| 20_laptop_open.png | 568470 | Fully open laptop; lid, display, keys and trackpad |
| 21_keyboard_backlight.png | 317115 | Night keyboard close-up with actual under-key light on |
| 22_clock.png | 136641 | Production clock after 17.25 seconds; all three hands |
| 23_showcase.png | 424921 | Actual showcase at 105.75 seconds; final overview and existing label/progress |
| 24_automatic_day_cycle.png | 348167 | Production day cycle on, after six seconds of sunset advance |
| 25_camera_tour.png | 734749 | Actual original camera tour at 11.5 seconds; existing TOUR ON label |
| 26_night_sky.png | 346000 | Clear moon/stars and illuminated skyline windows |
| 27_curtains_closed.png | 338124 | Same window pose as open curtains; closed, reduced daylight |
| 28_keyboard_backlight_off.png | 320452 | Exact same scene as image 21; only keyboard light off |
| 29_animation_paused.png | 804369 | Actual production pause, with existing PAUSED indicator |

The generated `report_screenshots/manifest.csv` records each image's resolution, size, camera/view, FOV, shading, environment, lamp switches, fan speed/angle, animation endpoints, keyboard state, clock, tour/showcase times and HUD preferences. Target columns record the camera's effective look direction as a point one unit in front of its position. PNGs and ZIP are ignored by Git and remain available locally.

## Feature inventory and additional discoveries

The final repository implements ceiling and bedside point lights, a study spotlight, curtain-dependent window daylight, Flat/Gouraud/Phong, fan power and four running speed levels with ramps, real hour/minute/second clock hands, a hinged room door, paired wardrobe doors, a sliding desk drawer, a laptop lid/display/keys/trackpad, keyboard light with automatic lid fading, folded swaying curtains, clear/rain weather, manual day/night and automatic day cycle. Rain dims daylight and increases curtain sway; the exterior contains sun/moon, stars, a skyline and night-lit windows.

Camera features include movement, mouse look, zoom/perspective, reset, cutaway overview, the original 32-second looping camera tour and the 109-second automated showcase. Pause/resume, HUD/help and internal F12 capture are present. Static procedural details include the bed, curved pillows, padded headboard, blanket folds, bedside table, chair, bookshelf/books/pages/ornament, floor planks, patterned/fringed rug, geometric artwork, notebook and pens.

Images 24-29 add the automatic cycle, original tour, night sky, closed-curtain comparison, keyboard-light-off comparison and pause state. Static chair/bedside drawer and fixed laptop screen artwork are represented as implemented. No plant or invented power/brightness controls were added.

## Capture implementation and consistency

`src/ReportCapture.cpp` holds the named pose table, ordered image table and isolated state presets. Every image begins with a fresh application/simulation state; all relevant switches and initial animation values are set explicitly. It advances the existing `updateApplication()`/`Simulation::update()` at 1/60-second steps until targets settle. Fan rotation and clock hands use their production equations. The showcase image starts the actual showcase controller, and the camera-tour image starts the original tour. No substitute animations are used.

Each state renders three identical frames without further time advance, finishes GPU work and reads the existing back framebuffer. `src/Screenshot.cpp` shares the original RGB readback and BMP writer with the PNG path, using the `stb_image_write.h` already vendored in GLFW. Rows are flipped for PNG orientation; file writing supports native filesystem paths. Existing F12/BMP verification behaviour remains intact. Capture restores the original application state before returning, including on a capture error.

Images 06-10 have exactly identical camera, target/direction, object animation positions, time, weather, curtains, shading and keyboard state. Only the three artificial-light switches change. Images 11-13 have exactly identical camera, targets, all object/light/environment states and timestamps; only shading changes. Images 21/28 differ only in keyboard backlight. Images 03/04 share exact camera framing. The script verifies these relationships against the complete CSV rows and checks that each comparison image differs.

HUD is hidden in clean scene/detail/comparison images. Images 15, 23, 24, 25 and 29 use the existing project HUD to show the relevant implemented state; image 15 also uses the existing help panel's fan-speed indication. No screenshot-specific scene labels or debug overlays are introduced.

## Named poses

| Pose | Position XYZ | Look-at target XYZ | FOV |
| --- | --- | --- | ---: |
| Overview / LightingComparison | 2.65, 2.50, 2.60 | 0, 1.05, -1.0 | 70 |
| BirdEye | 0, 7.4, 9.8 | 0, 0.8, -0.25 | 38 |
| ShadingComparison | 0.50, 1.8, 2.35 | -1.8, 0.8, -0.45 | 65 |
| Desk | 1.0, 2.10, -0.7 | 1.95, 0.9, -2.18 | 62 |
| Laptop | 1.0, 1.55, -1.15 | 1.60, 0.95, -2.16 | 48 |
| Keyboard | 1.28, 1.07, -1.87 | 1.60, 0.809, -2.199 | 45 |
| Fan | 1.0, 2.0, 1.65 | 0, 2.73, 0.1 | 58 |
| Door | 1.15, 2.35, 2.55 | 3.12, 1.08, 1.60 | 65 |
| Window | -0.1, 2.35, -0.65 | -0.05, 1.775, -2.8 | 65 |
| Weather | 0.35, 1.90, -1.30 | 0.25, 1.65, -2.70 | 68 |
| Wardrobe | -0.65, 2.25, -0.4 | -2.15, 1.1, -2.36 | 60 |
| Clock | 2.2, 2.5, -1.30 | 2.45, 2.64, -2.75 | 38 |

Showcase and original-tour viewpoints come from their existing production controllers.

## Validation

The Windows Release build succeeded without warnings on AMD Radeon 780M Graphics / OpenGL 3.3 core. All **131 existing application checks** passed, including manual controls, fan/backlight, shading, full showcase coverage/restoration and a 30-second render soak without OpenGL errors or additional mesh uploads.

All 29 images were visually reviewed. Every PNG was fully decoded and independently checked for chunk CRCs, valid complete compressed scanlines and the same 1920x1080 resolution. A separate capture from another working directory reproduced every PNG and the state manifest byte-for-byte. PNG and existing BMP captures of the same frame decoded to identical RGB pixels and orientation. The ZIP opens, passes CRC checks and contains exactly the 29 matching PNGs.

To merge the source changes later:

```bash
git checkout main
git pull
git merge feature/report-screenshots
```
