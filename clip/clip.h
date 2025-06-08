#pragma once

#include "../obj_loader/obj_loader.h"

struct lineClip {
  Vec3 v1;
  Vec3 v2;
  bool out;
  bool status;
};

struct VBuff {
  Vec3 v0;
  Vec3 v1;
  Vec3 v2;
  Vec3 color;
  VBuff(Vec3 _v0, Vec3 _v1, Vec3 _v2, Vec3 _c) : v0(_v0), v1(_v1), v2(_v2), color(_c) {}
};

float edgeFunc(const Vec3& v0, const Vec3& v1, const Vec3& v2);
void clip();

extern std::vector<VBuff> VOA;