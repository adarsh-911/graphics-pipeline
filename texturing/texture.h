#pragma once
#include <vector>

struct Color {
  unsigned char r, g, b;
};

struct pixelBuff {
  bool draw;
  Vec3 barycentric;
};

// Loads image and returns all pixel colors in row-major order (left-to-right, top-to-bottom)
std::vector<Color> loadTexture (const char* filename, int& width, int& height);
Color extractColor (int modelInd, int triInd, Vec3 barycentric, int x, int y, float z, int i);
Vec3 normalizeToPixel(const Vec3& screenCoord);