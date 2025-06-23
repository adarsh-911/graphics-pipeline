#pragma once

#include "../modelLoader/model.hpp"

struct pixelBuff {
  bool draw;
  glm::vec3 barycentric;
};

Color extractColor (int modelInd, int triInd, glm::vec3 barycentric, glm::vec3 persp_z, float z);