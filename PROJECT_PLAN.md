# My Smart Study Bedroom — Phase 1 Lite Demo

Status: Phase 1 Lite Demo complete and verified; ready for teacher progress demonstration.
Last updated: 2026-09-21.

## 1. Project overview

This is the existing C++ OpenGL 3.3 project, simplified for an initial teacher progress demonstration. The current deliverable is a recognizable bedroom and study room with basic models, camera navigation and Flat/Gouraud/Phong shading. It is not the previous fully detailed Phase 1 scene.

Keep OpenGL 3.3 Core Profile, GLFW, GLAD, GLM, C++17 and the existing procedural drawing functions. Keep the Shader, Camera, Mesh and DrawContext structure. No engine, new architecture or dependencies are introduced.

**Permanent scope decision: the plant is removed from the scene, source code and future roadmap. Do not restore it in later versions.** Other removed details may be extended later when requested.

## 2. Current scene and full feature roadmap

| Area | Phase 1 Lite Demo | Later full version, only when requested |
| --- | --- | --- |
| Room | Plain floor slab, walls, ceiling, openings and simple trim | Optional tile detail and textures |
| Bed | Frame, four legs, headboard, rounded mattress, one pillow and plain blanket | Richer bedding, folds and headboard detail |
| Bedside table | Top, four legs, simple drawer and handle | Optional finer details |
| Bedside lamp | Base, stem and shade; model only | Optional separate light source |
| Wardrobe | Body, base, two door panels and simple handles | Recesses, trim and optional interaction |
| Study table | Top and four legs | Optional drawer, study lamp and accessories |
| Chair | Seat, backrest and four legs | Optional rolling-chair model |
| Laptop | Base and screen panel only | Keyboard, bezel, trackpad and screen artwork |
| Bookshelf | Back, sides, shelves and three plain books | More detailed books and optional ornament |
| Window | Frame, sill, center bar and fixed blue backdrop | Optional exterior detail or transparent glass |
| Curtain | Two flat panels and a rod, stationary | Optional folds and animation |
| Wall clock | Face, hour marks and two stationary hands | Clock animation |
| Wall frame | Exactly one simple frame and colored inset | Optional artwork detail |
| Rug | One plain rectangular slab | Optional border or pattern |
| Door | Slab, frame and handle, closed and stationary | Hinge animation and optional panel detail |
| Ceiling fan | Stationary hub, rod and three blades | Rotor animation |
| Ceiling light | Optional static model retained | Optional controlled lighting |
| Lighting | One fixed point source plus ambient for shading comparison | Multiple lights, spotlights, attenuation, toggles and day/night lighting |
| Shading | 1 = Flat, 2 = Gouraud, 3 = Phong | Keep all three modes |
| Camera | Existing movement, mouse look, zoom, reset and overview | Optional collision or tour |
| Other detail | Dustbin, extra frame and tabletop clutter removed | May return selectively when requested |
| Plant | Permanently removed | Never restore |

All time-dependent behavior remains deferred: fan/door/curtain/clock animation, object interaction, weather and day/night cycle. There are no room-light or lamp toggles, spotlight cones or dynamic lighting controls in the Lite Demo.

## 3. Folder structure and explanation

The existing folder structure is preserved:

- `src/main.cpp`, `App.h`: context setup, controls, shader selection and frame loop.
- `src/Shader.*`: shader loading, diagnostics and uniform setters.
- `src/Camera.*`: view matrix, movement, mouse look and camera presets.
- `src/Mesh.*`: VAO/VBO/EBO ownership and smooth/flat mesh variants.
- `src/Primitives.*`: five reused shapes: box, rounded box, sphere, cylinder and frustum.
- `src/Material.h`, `Lighting.*`: named colors/materials and one fixed light.
- `src/Scene.cpp`: room-level placement and calls to object functions.
- `src/objects/Room.*`: shell, window/door openings and trim.
- `src/objects/Bedroom.*`: simplified bed, bedside table, wardrobe and lamp.
- `src/objects/Study.*`: simplified desk, chair, laptop and bookshelf.
- `src/objects/Decorations.*`: window, curtain, clock, one frame and rug.
- `src/objects/Fixtures.*`: static door, fan and ceiling-light model.
- `src/Verification.*`: existing optional captures and checks.
- `shaders/`: Flat, Gouraud, Phong and unlit shader pairs; GLSL 330 core.
- `external/`: existing pinned GLFW 3.4, GLM 1.0.1 and generated GLAD 2.0.8.
- `docs/screenshots/`, `docs/VERIFICATION.md`: current demo images and evidence.
- `CMakeLists.txt`, `build.bat`, `run.bat`: existing build and launch entry points.

Special folded-curtain, draped-blanket and artwork-triangle mesh generators were removed because their objects now use ordinary primitives. Flat and smooth variants are still uploaded once and selected at draw time.

## 4. Lighting and shading for viva

The fixed source has a position and color. Each material has a base color, specular strength and shininess. The equation is ambient + diffuse + specular. There is no spotlight, attenuation calculation or interaction state.

- **Flat:** use each triangle's face normal; pass its completed lit color without interpolation.
- **Gouraud:** calculate lighting at vertices and interpolate the resulting color.
- **Phong:** interpolate positions and normals, then calculate lighting per fragment.

All three paths use the same equation and fixed light data. The curved pillow and lamp are useful comparison objects even though most furniture is simple. Retain the inverse-transpose normal transform so non-uniformly scaled objects shade correctly. The fixed blue window backdrop uses the existing unlit shader. Lamps and the ceiling fixture use ordinary materials and do not have independent light behavior.

Explain the scene by following `drawScene` into `drawTable`: one tabletop plus a short loop for four legs. Each object's parts use parent * translation * rotation * scale. There are no new abstractions.

## 5. Implementation milestones

| Step | Work | State |
| --- | --- | --- |
| Earlier Phase 1 | Rich furnished environment, shaders, camera and reusable primitives | Previously built and verified; superseded as the active demo |
| Lite 1 | Inspect existing code and preserve build/camera/rendering structure | Complete |
| Lite 2 | Permanently remove plant and remove temporary clutter | Complete |
| Lite 3 | Simplify furniture, curtains, rug, floor and unused meshes | Complete |
| Lite 4 | Reduce lighting to one fixed source and keep 1/2/3 | Complete; runtime checks passed |
| Lite 5 | Build, run existing checks, inspect views and refresh documentation | Complete |

The worktree refactor leaves the existing staged richer files untouched. At inspection the repository had no commits, so the staged snapshot is not yet committed history. No staging, commits or resets are performed by this refactor.

## 6. Completed tasks checklist

- [x] Refactor the current files without redesigning the architecture.
- [x] Delete plant calls, declarations, implementation and unused green material.
- [x] Remove dustbin, study lamp, extra frame, shelf ornament and desk accessories.
- [x] Keep only three plain shelf books.
- [x] Replace rolling chair with seat, backrest and four legs.
- [x] Replace detailed laptop with base and screen.
- [x] Use plain curtain panels, rug, bedding and floor.
- [x] Simplify bedside furniture, wardrobe and door details.
- [x] Keep the bedside lamp as a three-part model only.
- [x] Remove spotlight uniforms, light-cone calculations and attenuation.
- [x] Keep one fixed light and the three mandatory shading modes.
- [x] Remove unused special mesh generators; preserve shared primitive helpers.
- [x] Add short TODOs at the relevant object/light functions.
- [x] Build the modified source successfully.
- [x] Pass shading, camera, resize, resource reuse and 30-second static-scene checks.
- [x] Inspect the Lite views and refresh README, verification notes and screenshots.

## 7. Remaining tasks checklist

- [ ] Demonstrate the Lite version to the teacher (user activity).
- [ ] When explicitly requested, extend selected details for the richer full version.
- [ ] When explicitly requested, implement Phase 2 motion, interaction and environment features.

Plant restoration is excluded from all future work.

## 8. Future extension plan

Short TODO comments identify future work in `Bedroom.cpp` (bedding), `Study.cpp` (books/desk details), `Decorations.cpp` (curtain folds), `Fixtures.cpp` (door/fan pivots) and `Lighting.cpp` (additional lights). These are future notes, not active features or commented-out implementations.

Keep extensions incremental: improve one object, verify the three shading modes, then document the change. Advanced lighting and animation require a later explicit request. No new dependencies or large abstractions are needed for this progress demonstration.

## 9. Progress record and maintenance

| Date | Step | Result | Next |
| --- | --- | --- | --- |
| 2026-09-21 | Earlier Phase 1 | Richer room implemented and verified | Superseded by teacher-demo simplification |
| 2026-09-21 | Lite modeling | Plant permanently removed, clutter deleted, furniture simplified, five shared shapes retained | Verify rendering |
| 2026-09-21 | Lite lighting | Single fixed source in all three shaders; no light controls or lamp behavior | Runtime and visual review |
| 2026-09-21 | Lite verification | Release build and existing runtime checks passed; five meshes, 116 interior draws (previously eight and 610); no OpenGL errors; static pixels unchanged after 30 seconds; six screenshots refreshed | Teacher demonstration; future additions require a request |

After each major implementation step, update this record and checklists with observed results. Do not restore the plant. Do not treat earlier full-version features as requirements for the Lite Demo.
