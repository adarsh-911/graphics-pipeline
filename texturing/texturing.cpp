#include "texturing.hpp"
#include "../vertexTransform/transform.hpp"

Color default_color = {20, 123, 67};

Color bilinear_filtering (int w, int h, float u, float v, const std::vector<Color>& texels) {
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

  glm::vec3 c00f(static_cast<float>(c00.r), static_cast<float>(c00.g), static_cast<float>(c00.b));
  glm::vec3 c10f(static_cast<float>(c10.r), static_cast<float>(c10.g), static_cast<float>(c10.b));
  glm::vec3 c01f(static_cast<float>(c01.r), static_cast<float>(c01.g), static_cast<float>(c01.b));
  glm::vec3 c11f(static_cast<float>(c11.r), static_cast<float>(c11.g), static_cast<float>(c11.b));

  glm::vec3 colorf = 
      c00f * (1 - fx) * (1 - fy) +
      c10f * fx * (1 - fy) +
      c01f * (1 - fx) * fy +
      c11f * fx * fy;

  Color color = {static_cast<u_char>(colorf.x), static_cast<u_char>(colorf.y), static_cast<u_char>(colorf.z)};
  return color;
}

Color uv_rounding(int w, int h, float u, float v, const std::vector<Color>& texels) {

  Color color = texels[std::clamp(0, static_cast<int>(round(u*(w-1)) + round(v*(h-1))*w), static_cast<int>(texels.size()-1))];
  return color;
}

Color extractColor (int modelIdx, int triIdx, glm::vec3 barycentric, glm::vec3 persp_w, float z) {
  idx idx1 = (models[modelIdx]).getIndices(2)[triIdx];
  std::vector<glm::vec2> texCoords = (models[modelIdx]).getTexCoords();

  if (texCoords.size() == 0) return default_color;

  int w =  models[modelIdx].texWidth[models[modelIdx].getTexID(triIdx)];
  int h = models[modelIdx].texHeight[models[modelIdx].getTexID(triIdx)];

  glm::vec2 v0 = texCoords[idx1.a];
  glm::vec2 v1 = texCoords[idx1.b];
  glm::vec2 v2 = texCoords[idx1.c];

  float w_intp = (barycentric.x/persp_w.x + barycentric.y/persp_w.y + barycentric.z/persp_w.z);
  float u = (barycentric.x*v0.x/persp_w.x + barycentric.y*v1.x/persp_w.y + barycentric.z*v2.x/persp_w.z)/w_intp;
  float v = (barycentric.x*v0.y/persp_w.x + barycentric.y*v1.y/persp_w.y + barycentric.z*v2.y/persp_w.z)/w_intp;

  u = std::clamp(0.0f, u, 1.0f);
  v = std::clamp(0.0f, v, 1.0f);
  
  //Color color = uv_rounding(w, h, u, v, models[modelIdx].getTexColors(models[modelIdx].getTexID(triIdx)));
  Color color = bilinear_filtering(w, h, u, v, models[modelIdx].getTexColors(models[modelIdx].getTexID(triIdx)));
  
  return color;
}