# Automated project showcase

Press **Y** to start or cancel. The complete unpaused timeline is **109 seconds**, below the 120-second limit. **P** pauses the timeline and all motion; **H** still toggles help, **F12** still captures a screenshot and **Escape** still exits. Mouse movement, zoom, free movement and other scene controls wait until the showcase exits.

## Repository inventory and coverage

| Existing feature | Showcase coverage |
| --- | --- |
| Camera movement/view/perspective and zoom | Eased elevated overview, named detail poses, varying FOV and the original camera-tour sample |
| Cutaway overview | Intro and final elevated orbit hide the front wall/ceiling through the existing overview flag |
| Ceiling point light, bedside point light, study spotlight | Each off/on alone; all eight switch combinations shown at night |
| Window daylight and distance/cone attenuation | Natural day with lamps off; curtains attenuate daylight; isolated lamps illuminate their receiving surfaces |
| Fan power and speed | Off -> Low 120 -> Medium 240 -> High 360 -> Max 480 -> Medium -> Off, in degrees/second, through the production 120 degrees/second squared ramp |
| Clock | Real second/minute/hour hands; five-second clock shot makes the second-hand motion obvious |
| Hinged room door | Closed -> opening -> fully open -> closing -> closed |
| Paired wardrobe doors | Closed -> open, revealing shelves/bedding -> closed |
| Sliding desk drawer and contents | Closed -> open alongside the study setup -> closed |
| Laptop lid, screen graphics, keys and trackpad | Closed -> open -> closed; existing display graphics visible while open |
| Keyboard backlight | Open laptop with lighting off/on; automatic brightness fade follows lid closure |
| Folded curtains | Closed -> open -> closed; daylight transmission changes and sway stays animated |
| Clear/rain weather | Clear -> rain -> clear; rainy-night combination, dimmer daylight and stronger curtain sway |
| Automatic day/night cycle | Real production cycle advances through sunset at its unchanged 0.1 hours/second |
| Sun/moon, stars, exterior skyline/windows | Window shots at day, sunset, clear night and rainy night |
| Flat/Gouraud/Phong | Two seconds each, with identical camera, lights and frozen simulation |
| Pause/resume | Frozen shading comparison then resumed animation; P can pause the entire showcase independently |
| Original 32-second camera tour | Three-second sample uses the existing tour calculation; it stops before the final overview |
| HUD/help and screenshot control | Existing visual style, demonstration titles, phase/time progress, intro help and usable H/F12 controls |
| Static procedural room details | Intro/overview and bed/study/wardrobe shots show floor planks, patterned/fringed rug, headboard, curved pillows, blanket folds, books/pages, ornament, geometric artwork and stationery |

Additional findings beyond the prompt's explicit feature list: rain increases curtain sway; sky geometry includes sun/moon, stars and skyline windows that illuminate at night; overview hides ceiling fixtures; screenshot capture/help controls and detailed procedural furnishings are present. The chair and bedside-table drawer are static models; no power switch or brightness-level control exists for the laptop display/backlight. The plant remains excluded. No invented controls or animations are added.

## Timeline

| Seconds | Phase | Actions |
| --- | --- | --- |
| 0-3 | Intro | Smooth elevated cutaway, help, neutral day |
| 3-8 | Day | Curtains open; natural daylight, all indoor lights off |
| 8-22 | Lighting | Smooth night transition; off, bedside, study, ceiling, ceiling+bedside, ceiling+study, bedside+study, all on; return to bird's-eye view for combinations |
| 22-29 | Study | Study spotlight; laptop and drawer open; drawer closes |
| 29-37 | Laptop/keyboard | Lid closes/opens/closes; backlight off/on and automatic fading |
| 37-53 | Fan | Off, Low, Medium, High, Max, reduction to Medium, coasting to Off |
| 53-58 | Clock | Close-up of real-time hands |
| 58-64 | Door | Closed/open/closed with hinge motion |
| 64-70 | Wardrobe | Paired doors open, interior visible, then close |
| 70-77 | Curtains | Closed/open/closed; daytime contribution; smooth sunset preparation |
| 77-83 | Automatic cycle | Real cycle advances from 17:06 to about 17:42; sun/moon/stars/skyline transition |
| 83-89 | Night | Dark room, bedside only, bedside+study, all lights on |
| 89-94 | Weather | Rainy warm-lit night with stronger curtain sway, then clear; prepare comparison pose |
| 94-100 | Shading/pause | Flat, Gouraud, Phong, two seconds each with frozen camera/scene/lighting |
| 100-103 | Original tour | Resume motion and sample the original camera tour with an eased entry |
| 103-106 | Final combination | Rainy night, all lamps, open laptop/backlight, Medium fan, clock, elevated orbit |
| 106-109 | Complete | Smooth camera/hour return; exact original user state restored at completion |

## Named camera waypoints

Positions and targets are local to the project's room coordinate system. Interior cameras stay below the ceiling. The cutaway stays enabled while entering/leaving elevated outside views.

| Pose | Position XYZ | Target XYZ | FOV |
| --- | --- | --- | --- |
| Overview | 0, 7.4, 9.8 | 0, 0.8, -0.25 | 48 |
| Room | 2.65, 2.70, 2.60 | 0, 1.1, -1 | 76 |
| Bed | -0.35, 2.40, 1.65 | -2.4, 0.8, -0.15 | 60 |
| Study | 1.0, 2.10, -0.7 | 1.95, 0.9, -2.18 | 62 |
| Keyboard | 1.28, 1.07, -1.87 | 1.60, 0.809, -2.199 | 45 |
| Fan | 1.0, 2.0, 1.65 | 0, 2.73, 0.1 | 65 |
| Clock | 1.8, 2.3, -1.0 | 2.45, 2.64, -2.75 | 42 |
| Door | 1.15, 2.35, 2.55 | 3.12, 1.08, 1.60 | 65 |
| Wardrobe | -0.65, 2.25, -0.4 | -2.15, 1.1, -2.36 | 60 |
| Window | -0.1, 2.35, -0.65 | -0.05, 1.775, -2.8 | 65 |
| RainWindow | -0.2, 1.95, -1.50 | -0.05, 1.775, -2.8 | 60 |
| Comparison | 0.50, 1.8, 2.35 | -1.8, 0.8, -0.45 | 76 |
| Finale | 3.8, 6.8, 7.8 | 0, 1.0, -0.4 | 52 |
| User return | Saved user camera | Saved viewing direction | Saved FOV |

## Controller and restoration

`src/Showcase.cpp` contains one duration table, one named pose table and one ordered event table. Event offsets are relative to phases, and compile-time checks enforce ordered events, valid offsets and a total below two minutes. `ShowcaseState::update()` splits updates at events and into at most 1/60-second simulation steps, making transitions deterministic even after a slow frame. The live timeline receives elapsed wall time rather than the manual-mode frame clamp, including while minimized.

Events set the existing Simulation target switches and fan levels. `Simulation::update()` remains the only owner of fan/hinge/drawer/lid motion, clock advance, environment time and rain/sway time. Smooth hour transitions do not alter wall-clock speed or the normal day-cycle rate. The original camera tour also retains its production calculation.

Starting saves the complete Simulation and Camera values, shading, tour/time and HUD/help preferences. Cancelling or completing copies them back, including ongoing animation positions and clocks. Pointer capture is preserved; the first mouse sample is reset on return to prevent a jump. Showcase pause has its own flag, allowing the timeline to present the production animation pause during shading comparison and still continue to completion.

Edit `phases`, `poses` and `events` in `src/Showcase.cpp` to change timing, viewpoints, targets or combinations. If shortening a phase, keep its event offsets inside its duration. The timeline logs each phase and completion once, without per-frame output. F12 remains available throughout; screenshots are not automatically written during an ordinary showcase.

## Verification

The Windows build succeeded on 2026-10-06. Running `build/smart_study_bedroom.exe --verify` passed all 131 checks on AMD Radeon 780M Graphics with an OpenGL 3.3 core context. The suite includes complete phase/state coverage, all eight lamp combinations, all fan speed levels and smooth ramps, furniture endpoints, keyboard lighting, real clock/day-cycle advance, camera continuity, a frozen shading comparison, cancellation during multiple phases and exact restoration of a custom state. Bulk, 30 FPS and 144 FPS updates agree. The 30-second render soak completed without OpenGL errors or additional mesh uploads.

A separate native-window run used actual keyboard events. Y started and cancelled the showcase; a full uninterrupted sequence finished in **109.089 measured wall-clock seconds** against the **109-second timeline**. Both cancellation and automatic completion restored a byte-identical screenshot of a custom paused scene. Manual shading and camera reset worked afterward, and Escape exited cleanly during a restarted showcase. Seventeen phase screenshots were reviewed for framing and HUD readability.

No existing interactive feature was omitted. The original looping camera tour is sampled for three seconds rather than playing its entire 32-second loop, keeping the full demonstration within the target duration. Static furniture and the laptop's fixed screen artwork are shown without inventing controls that the project does not implement.
