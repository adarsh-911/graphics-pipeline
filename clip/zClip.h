#pragma once

#include "../objLoader/obj_loader.h"

struct lineClip1 {
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
  int modelInd;
  int triInd;
  VBuff(Vec3 _v0, Vec3 _v1, Vec3 _v2, Vec3 _c, int _i, int _j) : v0(_v0), v1(_v1), v2(_v2), color(_c), modelInd(_i), triInd(_j) {}
};

extern void zClip(int bypass);
extern void VertexBuffer_update();
extern std::vector<VBuff> VOA;