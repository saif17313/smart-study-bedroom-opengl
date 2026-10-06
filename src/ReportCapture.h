#pragma once
#include "App.h"
#include <filesystem>
#include <functional>

void captureReport(GLFWwindow* window, AppState& app, const std::function<void()>& render,
                   const std::filesystem::path& directory);
