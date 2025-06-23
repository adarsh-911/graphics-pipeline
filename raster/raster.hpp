#pragma once
#include <string>
#include "../screen.hpp"

void render();
void savePPM(const std::string& filename);

extern unsigned char framebuffer[Screen::HEIGHT][Screen::WIDTH][3];