#pragma once
#include "App.h"
#include <filesystem>
#include <functional>

void saveScreenshot(const std::filesystem::path& path, int width, int height);
void verifyApplication(GLFWwindow* window, AppState& app, const std::function<void()>& render,
                       const std::filesystem::path& outputDirectory);
