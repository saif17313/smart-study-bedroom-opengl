#pragma once
#include <filesystem>
#include <vector>

// RGB rows follow OpenGL's bottom-to-top framebuffer order.
std::vector<unsigned char> readFrame(int width, int height);
void saveScreenshot(const std::filesystem::path& path, int width, int height);
