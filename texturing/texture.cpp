#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>
#include "stb_image.h"
#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"
#include "../clip/clip.h"
#include "texture.h"

const int WIDTH = 600;
const int HEIGHT = 600;

Vec3 normalizeToPixel(const Vec3& screenCoord) {
  float x = (screenCoord.x + 1.0f) * 0.5f * (WIDTH - 1);   // Maps x from [-1, 1] to [0, WIDTH-1]
  float y = (screenCoord.y + 1.0f) * 0.5f * (HEIGHT - 1);  // Maps y from [-1, 1] to [0, HEIGHT-1]
  float z = (screenCoord.z + 1.0f) * 0.5f; // Maps z from [-1, 1] to [0, 1]
  return Vec3(x, y, z);
}

Vec3 find_barycentric(int x, int y, Vec3 v0, Vec3 v1, Vec3 v2) {
  float px = x + 0.5f;
  float py = y + 0.5f;
  float w0 = 0, w1 = 0, w2 = 0;
  float area = edgeFunc(v0, v1, v2);
  if (area != 0) {
    w0 = edgeFunc(Vec3(px, py, 1.0f), v1, v2)/area;
    w1 = edgeFunc(Vec3(px, py, 1.0f), v2, v0)/area;
    w2 = edgeFunc(Vec3(px, py, 1.0f), v0, v1)/area;
  }
  return Vec3(w0, w1, w2);
}

std::vector<Color> loadTexture (const char* filename, int& width, int& height) {
  int channels;
  unsigned char* data = stbi_load(filename, &width, &height, &channels, 3); // force RGB
  if (!data) {
      std::cerr << "Failed to load image: " << filename << "\n";
      return {};
  }
  std::vector<Color> pixels;
  pixels.reserve(width * height);
  for (int i = 0; i < width * height * 3; i += 3) {
      Color c = { data[i], data[i + 1], data[i + 2] };
      pixels.push_back(c);
  }
  stbi_image_free(data);
  return pixels;
}

Color extractColor (int modelInd, int triInd, Vec3 barycentric, int x, int y, float z, int i) {
  Ind idx = (modelTexCordsInd[modelInd]).idx[triInd];
  std::vector<Vec2uv> texCords = (modelTexCords[modelInd]).texcoords;

  Ind idx1 = (modelTriangleInd[modelInd]).idx[triInd];
  std::vector<glm::vec4> vertices = (modelsCam[modelInd].vertices);

  Vec3 ver0 = normalizeToPixel(Vec3(vertices[idx1.v0].x/vertices[idx.v0].w, vertices[idx1.v0].y/vertices[idx.v0].w, vertices[idx1.v0].z/vertices[idx.v0].w));
  Vec3 ver1 = normalizeToPixel(Vec3(vertices[idx1.v1].x/vertices[idx.v1].w, vertices[idx1.v1].y/vertices[idx.v1].w, vertices[idx1.v1].z/vertices[idx.v1].w));
  Vec3 ver2 = normalizeToPixel(Vec3(vertices[idx1.v2].x/vertices[idx.v2].w, vertices[idx1.v2].y/vertices[idx.v2].w, vertices[idx1.v2].z/vertices[idx.v2].w));

  if (clipStatus[i]) barycentric = find_barycentric(x, y, ver0, ver1, ver2);

  int w =  modelTexColors[modelInd].width;
  int h = modelTexColors[modelInd].height;

  Vec2uv v0 = texCords[idx.v0];
  Vec2uv v1 = texCords[idx.v1];
  Vec2uv v2 = texCords[idx.v2];
  
  float u = (barycentric.x*v0.u/ver0.z + barycentric.y*v1.u/ver1.z + barycentric.z*v2.u/ver2.z)*z;
  float v = (barycentric.x*v0.v/ver0.z + barycentric.y*v1.v/ver1.z + barycentric.z*v2.v/ver2.z)*z;

  u = std::clamp(0.0f, u, 1.0f);
  v = std::clamp(0.0f, v, 1.0f);
  
  int size = modelTexColors[modelInd].pixels.size();

  Color color = modelTexColors[modelInd].pixels[std::clamp(0, static_cast<int>(round(u*(w-1)) + round(v*(h-1))*w), size -1)];
  
  return color;
}