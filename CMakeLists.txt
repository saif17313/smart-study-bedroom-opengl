cmake_minimum_required(VERSION 3.20)
project(SmartStudyBedroom VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(smart_study_bedroom
    src/main.cpp src/Shader.cpp src/Camera.cpp src/Mesh.cpp
    src/Primitives.cpp src/Lighting.cpp src/Scene.cpp src/Verification.cpp
    src/objects/Room.cpp src/objects/Bedroom.cpp src/objects/Study.cpp
    src/objects/Decorations.cpp src/objects/Fixtures.cpp)
target_include_directories(smart_study_bedroom PRIVATE src external/glm)
target_link_libraries(smart_study_bedroom PRIVATE glad glfw ${CMAKE_DL_LIBS})
target_compile_definitions(smart_study_bedroom PRIVATE GLFW_INCLUDE_NONE)
if(MSVC)
    target_compile_options(smart_study_bedroom PRIVATE /W4)
else()
    target_compile_options(smart_study_bedroom PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(MINGW)
    # A copied executable should not need GCC runtime DLLs on another Windows PC.
    target_link_options(smart_study_bedroom PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Always copy, including when only a shader changes. Launch from the build folder.
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:smart_study_bedroom>/shaders")
add_dependencies(smart_study_bedroom copy_shaders)
