#pragma once
#include <string>

const int WIDTH = 600;
const int HEIGHT = 600;

void render();
void savePPM(const std::string& filename);

extern unsigned char framebuffer[HEIGHT][WIDTH][3];