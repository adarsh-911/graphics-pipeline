#pragma once

#include "../modelLoader/model.hpp"
#include "../vertexTransform/transform.hpp"

struct ZBuff {
  glm::vec4 v0;
  glm::vec4 v1;
  glm::vec4 v2;
  Color color;
  int modelIdx;
  int triIdx;
  ZBuff() = default;
  ZBuff(glm::vec4 _v0, glm::vec4 _v1, glm::vec4 _v2, Color _c, int _i, int _j) : v0(_v0), v1(_v1), v2(_v2), color(_c), modelIdx(_i), triIdx(_j) {}
};

struct zlineClip {
  glm::vec4 v1;
  glm::vec4 v2;
  bool out;
  bool status;
  zlineClip(glm::vec4 _v1 = {0.0f, 0.0f, 0.0f, 0.0f}, glm::vec4 _v2 = {0.0f, 0.0f, 0.0f, 0.0f}, bool _o = false, bool _s = false) : v1(_v1), v2(_v2), out(_o), status(_s) {}
};

void zClip(bool bypass);
extern std::vector<ZBuff> zclipSpace;