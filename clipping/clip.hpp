#pragma once

#include "../vertexTransform/transform.hpp"

struct lineClip {
  glm::vec3 v1;
  glm::vec3 v2;
  bool out;
  bool status;
};

struct VBuff {
  glm::vec3 v0;
  glm::vec3 v1;
  glm::vec3 v2;
  Color color;
  int modelIdx;
  int triIdx;
  VBuff(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, Color _c, int _i, int _j) : v0(_v0), v1(_v1), v2(_v2), color(_c), modelIdx(_i), triIdx(_j) {}
};

float edgeFunc(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
void clip();

extern std::vector<VBuff> VOA;