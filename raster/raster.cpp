#include <fstream>
#include "../modelLoader/loadModels.hpp"
#include "../vertexTransform/transform.hpp"
#include "../clipping/clip.hpp"
#include "../texturing/texturing.hpp"
#include "../lightning/lightning.hpp"
#include "raster.hpp"

bool TEXTURE = true;
bool LIGHTNING = true;

unsigned char framebuffer[Screen::HEIGHT][Screen::WIDTH][3] = {};  // initialized to black
float zBuffer[Screen::HEIGHT][Screen::WIDTH] = {};

glm::vec3 persp_tex_z;
glm::mat4 WORLD_TO_SCREEN_INV;

void zBuffInit() {
  for (int i = 0; i < Screen::HEIGHT; ++i)
    for (int j = 0; j < Screen::WIDTH; ++j)
        zBuffer[i][j] = INFINITY;
}

void generateINV() {
  WORLD_TO_SCREEN_INV = glm::inverse(WORLD_TO_SCREEN);
}

glm::vec3 normalizeToPixel(const glm::vec3& screenCoord) {
  float x = (screenCoord.x + 1.0f) * 0.5f * (Screen::WIDTH - 1);
  float y = ((screenCoord.y + 1.0f) * 0.5f) * (Screen::HEIGHT - 1);
  float z = (screenCoord.z + 1.0f) * 0.5f;
  return glm::vec3(x, y, z);
}

glm::vec3 find_barycentric (int x, int y, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) {
  float px = x + 0.5f;
  float py = y + 0.5f;
  float w0 = 0, w1 = 0, w2 = 0;
  float area = edgeFunc(v0, v1, v2);
  if (area != 0) {
    w0 = edgeFunc(glm::vec3(px, py, 1.0f), v1, v2)/area;
    w1 = edgeFunc(glm::vec3(px, py, 1.0f), v2, v0)/area;
    w2 = edgeFunc(glm::vec3(px, py, 1.0f), v0, v1)/area;
  }
  return glm::vec3(w0, w1, w2);
}

glm::vec3 extractBarycentric(int modelIdx, int triIdx, float x, float y) {
  idx idx1 = (models[modelIdx]).getIndices(1)[triIdx];
  glm::vec4* vertices = (cameraSpace[modelIdx].vertices).data();

  glm::vec3 ver0 = normalizeToPixel(glm::vec3(vertices[idx1.a].x/vertices[idx1.a].w, vertices[idx1.a].y/vertices[idx1.a].w, vertices[idx1.a].z/vertices[idx1.a].w));
  glm::vec3 ver1 = normalizeToPixel(glm::vec3(vertices[idx1.b].x/vertices[idx1.b].w, vertices[idx1.b].y/vertices[idx1.b].w, vertices[idx1.b].z/vertices[idx1.b].w));
  glm::vec3 ver2 = normalizeToPixel(glm::vec3(vertices[idx1.c].x/vertices[idx1.c].w, vertices[idx1.c].y/vertices[idx1.c].w, vertices[idx1.c].z/vertices[idx1.c].w));

  glm::vec3 barycentric = find_barycentric(x, y, ver0, ver1, ver2);
  persp_tex_z = glm::vec3(ver0.z, ver1.z, ver2.z);
  return barycentric;
}

pixelBuff insideTriangle(int x, int y, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
  float px = x + 0.5f;
  float py = y + 0.5f;
  float w0 = 0, w1 = 0, w2 = 0, update = 0, z;
  float area = edgeFunc(v0, v1, v2);
  if (area != 0) {
    w0 = edgeFunc(glm::vec3(px, py, 1.0f), v1, v2)/area;
    w1 = edgeFunc(glm::vec3(px, py, 1.0f), v2, v0)/area;
    w2 = edgeFunc(glm::vec3(px, py, 1.0f), v0, v1)/area;
    z = 1 / (w0*(1/v0.z) + w1*(1/v1.z) + w2*(1/v2.z));
    if (zBuffer[y][x] > z) update = 1;
  }
  pixelBuff pixel;
  bool write = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
  if (write) { 
    if (update == 1) {
      zBuffer[y][x] = z;
      pixel.draw = true;
      pixel.barycentric = glm::vec3(w0, w1, w2);
    } 
    else {
      pixel.draw = false;
      pixel.barycentric = glm::vec3(w0, w1, w2);
    }
  }
  else {
    pixel.draw = false;
    pixel.barycentric = glm::vec3(w0, w1, w2);
  }
  return pixel;
}

void drawTriangle(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, Color color, int modelIdx, int triIdx) {

  glm::vec3 v0Pixel = normalizeToPixel(v0);
  glm::vec3 v1Pixel = normalizeToPixel(v1);
  glm::vec3 v2Pixel = normalizeToPixel(v2);

  int minX = std::max(0, (int)std::floor(std::min(std::min(v0Pixel.x, v1Pixel.x), v2Pixel.x)));
  int maxX = std::min(Screen::WIDTH - 1, (int)std::ceil(std::max(std::max(v0Pixel.x, v1Pixel.x), v2Pixel.x)));
  int minY = std::max(0, (int)std::floor(std::min(std::min(v0Pixel.y, v1Pixel.y), v2Pixel.y)));
  int maxY = std::min(Screen::HEIGHT - 1, (int)std::ceil(std::max(std::max(v0Pixel.y, v1Pixel.y), v2Pixel.y)));

  for (int y = minY; y <= maxY; ++y) {
    for (int x = minX; x <= maxX; ++x) {
      pixelBuff currentPixel = insideTriangle(x, y, v0Pixel, v1Pixel, v2Pixel);
      if (currentPixel.draw) {
        
        if (TEXTURE and LIGHTNING) {
          glm::vec3 bary = extractBarycentric(modelIdx, triIdx, x, y);
          Color texColor = extractColor(modelIdx, triIdx, bary, persp_tex_z, zBuffer[y][x]);
          glm::vec3 lightAmount = lightIntensity(modelIdx, triIdx, bary, WORLD_TO_SCREEN_INV);
          framebuffer[y][x][0] = std::min(static_cast<float>(texColor.r)*lightAmount.x, 255.0f);
          framebuffer[y][x][1] = std::min(static_cast<float>(texColor.g)*lightAmount.y, 255.0f);
          framebuffer[y][x][2] = std::min(static_cast<float>(texColor.b)*lightAmount.z, 255.0f);
        }

        else if (TEXTURE) {
          glm::vec3 bary = extractBarycentric(modelIdx, triIdx, x, y);
          Color texColor = extractColor(modelIdx, triIdx, bary, persp_tex_z, zBuffer[y][x]);
          framebuffer[y][x][0] = std::min(static_cast<float>(texColor.r), 255.0f);
          framebuffer[y][x][1] = std::min(static_cast<float>(texColor.g), 255.0f);
          framebuffer[y][x][2] = std::min(static_cast<float>(texColor.b), 255.0f);
        }

        else if (LIGHTNING) {
          glm::vec3 bary = extractBarycentric(modelIdx, triIdx, x, y);
          glm::vec3 lightAmount = lightIntensity(modelIdx, triIdx, bary, WORLD_TO_SCREEN_INV);
          framebuffer[y][x][0] = std::min(static_cast<float>(color.r)*lightAmount.x, 255.0f);
          framebuffer[y][x][1] = std::min(static_cast<float>(color.g)*lightAmount.y, 255.0f);
          framebuffer[y][x][2] = std::min(static_cast<float>(color.b)*lightAmount.z, 255.0f);
        }

        else {
          framebuffer[y][x][0] = static_cast<float>(color.r);  //R
          framebuffer[y][x][1] = static_cast<float>(color.g);  //G
          framebuffer[y][x][2] = static_cast<float>(color.b);  //B
        }
        
      }
    }
  }
}

void savePPM(const std::string& filename) {
  std::ofstream ofs(filename, std::ios::binary);
  ofs << "P6\n" << Screen::WIDTH << " " << Screen::HEIGHT << "\n255\n";
  for (int y = 0; y < Screen::HEIGHT; ++y)
    for (int x = 0; x < Screen::WIDTH; ++x)
      ofs.write((char*)framebuffer[y][x], 3);
}

void render() {
  zBuffInit();
  if (LIGHTNING) generateINV();
  for (VBuff& v : VOA) drawTriangle(v.v0, v.v1, v.v2, v.color, v.modelIdx, v.triIdx);
}
