# Local dependencies

The project builds from these local files without contacting a package server.

| Library | Version | Source | License |
| --- | --- | --- | --- |
| GLFW | 3.4 | https://github.com/glfw/glfw/releases/tag/3.4 | `glfw/LICENSE.md` |
| GLM | 1.0.1 | https://github.com/g-truc/glm/releases/tag/1.0.1 | `glm/copying.txt` |
| GLAD | Generator 2.0.8; desktop OpenGL 3.3 core, no extensions | https://github.com/Dav1dde/glad/releases/tag/v2.0.8 | `glad/LICENSE` and generated file notices |

GLFW and GLM are extracted release source archives. GLFW is compiled by CMake with the same compiler as the application. GLM is used as headers only. GLAD's matching `include/glad/gl.h`, `include/KHR/khrplatform.h` and `src/gl.c` are generated files and are retained here; Python is not needed to build or run the project.

GLAD generation used its bundled specifications (`--reproducible`), C output, API `gl:core=3.3`, and an empty extension list. The generation command was `python -m glad --api gl:core=3.3 --extensions= --out-path external/glad --reproducible c` with glad2 2.0.8. Never combine these GLAD 2 files with GLAD 1 initialization examples.

The ignored `.tools` and `.downloads` directories are local setup artifacts, not runtime dependencies. Keep the dependency license files when redistributing the source.
