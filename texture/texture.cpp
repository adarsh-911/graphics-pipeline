#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../obj_loader/obj_loader.h"
#include "../shader/shader.h"
#include "texture.h"

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

Color extractColor (int modelInd, int triInd, Vec3 barycentric) {
  modelTexIndClass texIndInst = modelTexCordsInd[modelInd];
  Ind idx = texIndInst.texInds[triInd];
  modelTexClass texInst = modelTexCords[modelInd];
  std::vector<Vec2uv> texCords = texInst.texcoords;
  int w =  modelTexColors[modelInd].width;
  int h = modelTexColors[modelInd].height;
  //std::cout << w << " " << h << "\n";
  //std::cout << idx.v0 << " " << idx.v1 << " " << idx.v2 << "\n";
  //std::cout << texInst.name << "\n";
  Vec2uv v0 = texCords[idx.v0];
  Vec2uv v1 = texCords[idx.v1];
  Vec2uv v2 = texCords[idx.v2];
  float u = barycentric.x*v0.u + barycentric.y*v1.u + barycentric.z*v2.u;
  float v = barycentric.x*v0.v + barycentric.y*v1.v + barycentric.z*v2.v;
  //std::cout << u << " " << v << "\n";
  //std::cout << std::round(u*(w-1)) + std::round(v*(h-1))*w << "\n";
  Color color = modelTexColors[modelInd].pixels[round(u*(w-1)) + round(v*(h-1))*w];
  return color;
}