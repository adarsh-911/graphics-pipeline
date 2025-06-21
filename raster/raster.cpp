#include <fstream>
#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"
#include "../clip/clip.h"
#include "../texturing/texture.h"
#include "../lightning/lightning.h"
#include "raster.h"

bool TEXTURE = true;
bool LIGHTNING = true;

const int WIDTH = 600;
const int HEIGHT = 600;

unsigned char framebuffer[HEIGHT][WIDTH][3] = {};  // initialized to black
float zBuffer[HEIGHT][WIDTH] = {};
std::vector<bool> clipStatus1;

void zBuffInit() {
  for (int i = 0; i < HEIGHT; ++i)
    for (int j = 0; j < WIDTH; ++j)
        zBuffer[i][j] = INFINITY;
}

pixelBuff insideTriangle(int x, int y, const Vec3& v0, const Vec3& v1, const Vec3& v2) {
  float px = x + 0.5f;
  float py = y + 0.5f;
  float w0 = 0, w1 = 0, w2 = 0, update = 0, z;
  float area = edgeFunc(v0, v1, v2);
  if (area != 0) {
    w0 = edgeFunc(Vec3(px, py, 1.0f), v1, v2)/area;
    w1 = edgeFunc(Vec3(px, py, 1.0f), v2, v0)/area;
    w2 = edgeFunc(Vec3(px, py, 1.0f), v0, v1)/area;
    z = 1 / (w0*(1/v0.z) + w1*(1/v1.z) + w2*(1/v2.z));
    if (zBuffer[y][x] > z) update = 1;
  }
  pixelBuff pixel;
  bool write = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
  if (write) { 
    if (update == 1) {
      zBuffer[y][x] = z;
      pixel.draw = true;
      pixel.barycentric = Vec3(w0, w1, w2);
    } 
    else {
      pixel.draw = false;
      pixel.barycentric = Vec3(w0, w1, w2);
    }
  }
  else {
    pixel.draw = false;
    pixel.barycentric = Vec3(w0, w1, w2);
  }
  return pixel;
}

void drawTriangle(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 color, int modelInd, int triInd, int i) {

  Vec3 v0Pixel = normalizeToPixel(v0);
  Vec3 v1Pixel = normalizeToPixel(v1);
  Vec3 v2Pixel = normalizeToPixel(v2);

  int minX = std::max(0, (int)std::floor(std::min(std::min(v0Pixel.x, v1Pixel.x), v2Pixel.x)));
  int maxX = std::min(WIDTH - 1, (int)std::ceil(std::max(std::max(v0Pixel.x, v1Pixel.x), v2Pixel.x)));
  int minY = std::max(0, (int)std::floor(std::min(std::min(v0Pixel.y, v1Pixel.y), v2Pixel.y)));
  int maxY = std::min(HEIGHT - 1, (int)std::ceil(std::max(std::max(v0Pixel.y, v1Pixel.y), v2Pixel.y)));

  for (int y = minY; y <= maxY; ++y) {
    for (int x = minX; x <= maxX; ++x) {
      pixelBuff currentPixel = insideTriangle(x, y, v0Pixel, v1Pixel, v2Pixel);
      if (currentPixel.draw) {
        
        if (TEXTURE and LIGHTNING) {
          Color texColor = extractColor(modelInd, triInd, currentPixel.barycentric, x, y, zBuffer[y][x], i);
          glm::vec3 lightAmount = lightIntensity(modelInd, triInd, currentPixel.barycentric);
          framebuffer[y][x][0] = std::min(static_cast<float>(texColor.r)*lightAmount.x, 255.0f);
          framebuffer[y][x][1] = std::min(static_cast<float>(texColor.g)*lightAmount.y, 255.0f);
          framebuffer[y][x][2] = std::min(static_cast<float>(texColor.b)*lightAmount.z, 255.0f);
        }

        else if (TEXTURE) {
          Color texColor = extractColor(modelInd, triInd, currentPixel.barycentric, x, y, zBuffer[y][x], i);
          framebuffer[y][x][0] = std::min(static_cast<float>(texColor.r), 255.0f);
          framebuffer[y][x][1] = std::min(static_cast<float>(texColor.g), 255.0f);
          framebuffer[y][x][2] = std::min(static_cast<float>(texColor.b), 255.0f);
        }

        else if (LIGHTNING) {
          glm::vec3 lightAmount = lightIntensity(modelInd, triInd, currentPixel.barycentric);
          framebuffer[y][x][0] = std::min(color.x*lightAmount.x, 255.0f);
          framebuffer[y][x][1] = std::min(color.y*lightAmount.y, 255.0f);
          framebuffer[y][x][2] = std::min(color.z*lightAmount.z, 255.0f);
        }

        else {
          framebuffer[y][x][0] = color.x;  //R
          framebuffer[y][x][1] = color.y;  //G
          framebuffer[y][x][2] = color.z;  //B
        }
        
      }
    }
  }
}

void savePPM(const std::string& filename) {
  std::ofstream ofs(filename, std::ios::binary);
  ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
  for (int y = 0; y < HEIGHT; ++y)
    for (int x = 0; x < WIDTH; ++x)
      ofs.write((char*)framebuffer[y][x], 3);
}

void render() {
  zBuffInit();
  int i = 0;
  for (VBuff& v : VOA) drawTriangle(v.v0, v.v1, v.v2, v.color, v.modelInd, v.triInd, i); i++;
}