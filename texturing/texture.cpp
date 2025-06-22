#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>
#include "stb_image.h"
#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"
#include "../clip/clip.h"
#include "../clip/zClip.h"
#include "texture.h"

const int WIDTH = 600;
const int HEIGHT = 600;

Vec3 normalizeToPixel(const Vec3& screenCoord) {
  float x = (screenCoord.x + 1.0f) * 0.5f * (WIDTH - 1);   // Maps x from [-1, 1] to [0, WIDTH-1]
  float y = (screenCoord.y + 1.0f) * 0.5f * (HEIGHT - 1);  // Maps y from [-1, 1] to [0, HEIGHT-1]
  float z = (screenCoord.z + 1.0f) * 0.5f; // Maps z from [-1, 1] to [0, 1]
  return Vec3(x, y, z);
}

Color bilinear_filtering(int w, int h, float u, float v, std::vector<Color>& texels) {
  float tex_x = u * (w - 1);
  float tex_y = v * (h - 1);

  float fx = tex_x - floor(tex_x);
  float fy = tex_y - floor(tex_y);
  int x0 = static_cast<int>(floor(tex_x));
  int y0 = static_cast<int>(floor(tex_y));
  int x1 = std::min(x0 + 1, w - 1);
  int y1 = std::min(y0 + 1, h - 1);

  Color c00 = texels[y0 * w + x0];
  Color c10 = texels[y0 * w + x1];
  Color c01 = texels[y1 * w + x0];
  Color c11 = texels[y1 * w + x1];

  Vec3 c00f(static_cast<float>(c00.r), static_cast<float>(c00.g), static_cast<float>(c00.b));
  Vec3 c10f(static_cast<float>(c10.r), static_cast<float>(c10.g), static_cast<float>(c10.b));
  Vec3 c01f(static_cast<float>(c01.r), static_cast<float>(c01.g), static_cast<float>(c01.b));
  Vec3 c11f(static_cast<float>(c11.r), static_cast<float>(c11.g), static_cast<float>(c11.b));

  Vec3 colorf = 
      c00f * (1 - fx) * (1 - fy) +
      c10f * fx * (1 - fy) +
      c01f * (1 - fx) * fy +
      c11f * fx * fy;

  Color color = {static_cast<u_char>(colorf.x), static_cast<u_char>(colorf.y), static_cast<u_char>(colorf.z)};
  return color;
}

Color uv_rounding(int w, int h, float u, float v, std::vector<Color>& texels) {

  Color color = texels[std::clamp(0, static_cast<int>(round(u*(w-1)) + round(v*(h-1))*w), static_cast<int>(texels.size()-1))];
  return color;
}

Color extractColor (int modelInd, int triInd, Vec3 barycentric, float z) {
  Ind idx = (modelTexCordsInd[modelInd]).idx[triInd];
  std::vector<Vec2uv> texCords = (modelTexCords[modelInd]).texcoords;

  Ind idx1 = (modelTriangleInd[modelInd]).idx[triInd];
  std::vector<glm::vec4> vertices = (modelsCam[modelInd].vertices);

  Vec3 ver0 = normalizeToPixel(Vec3(vertices[idx1.v0].x/vertices[idx1.v0].w, vertices[idx1.v0].y/vertices[idx1.v0].w, vertices[idx1.v0].z/vertices[idx1.v0].w));
  Vec3 ver1 = normalizeToPixel(Vec3(vertices[idx1.v1].x/vertices[idx1.v1].w, vertices[idx1.v1].y/vertices[idx1.v1].w, vertices[idx1.v1].z/vertices[idx1.v1].w));
  Vec3 ver2 = normalizeToPixel(Vec3(vertices[idx1.v2].x/vertices[idx1.v2].w, vertices[idx1.v2].y/vertices[idx1.v2].w, vertices[idx1.v2].z/vertices[idx1.v2].w));

  //barycentric = find_barycentric(x, y, ver0, ver1, ver2);

  int w =  modelTexColors[modelInd].width;
  int h = modelTexColors[modelInd].height;

  Vec2uv v0 = texCords[idx.v0];
  Vec2uv v1 = texCords[idx.v1];
  Vec2uv v2 = texCords[idx.v2];
  
  float u = (barycentric.x*v0.u/ver0.z + barycentric.y*v1.u/ver1.z + barycentric.z*v2.u/ver2.z)*z;
  float v = (barycentric.x*v0.v/ver0.z + barycentric.y*v1.v/ver1.z + barycentric.z*v2.v/ver2.z)*z;

  u = std::clamp(0.0f, u, 1.0f);
  v = std::clamp(0.0f, v, 1.0f);

  //Color color = uv_rounding(w, h, u, v, modelTexColors[modelInd].pixels);
  Color color = bilinear_filtering(w, h, u, v, modelTexColors[modelInd].pixels);
  return color;
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