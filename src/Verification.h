#pragma once
#include "App.h"
#include "Screenshot.h"
#include <filesystem>
#include <functional>

void verifyApplication(GLFWwindow* window, AppState& app, const std::function<void()>& render,
                       const std::filesystem::path& outputDirectory);
