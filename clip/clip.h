#pragma once

#include "../objLoader/obj_loader.h"

struct lineClip {
  Vec4 v1;
  Vec4 v2;
  bool out;
  bool status;
};

struct VBuff1 {
  Vec4 v0;
  Vec4 v1;
  Vec4 v2;
  Vec3 color;
  int modelInd;
  int triInd;
  VBuff1(Vec4 _v0, Vec4 _v1, Vec4 _v2, Vec3 _c, int _i, int _j) : v0(_v0), v1(_v1), v2(_v2), color(_c), modelInd(_i), triInd(_j) {}
};

float edgeFunc(const Vec3& v0, const Vec3& v1, const Vec3& v2);
bool floatEq(float x, float y);
void clip();

extern std::vector<VBuff1> VOA_temp;