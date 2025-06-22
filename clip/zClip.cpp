#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"
#include "clip.h"
#include "zClip.h"

std::vector<VBuff> VOA;
std::vector<VBuff> VOA_temp2;

lineClip1 liang_barskyClip_xz (const Vec4& ver1, const Vec4& ver2) {
  lineClip1 clipped;
  clipped.v1 = Vec3(ver1.x/ver1.w, ver1.y/ver1.w, ver1.z/ver1.w);
  clipped.v2 = Vec3(ver2.x/ver2.w, ver2.y/ver2.w, ver2.z/ver2.w);
  clipped.status = false;

  if ((fabsf(ver1.x) < ver1.w) && (fabsf(ver1.y) < ver1.w) && (fabsf(ver2.x) < ver2.w) && (fabsf(ver2.y) < ver2.w)) {
    clipped.out = false; 
    return clipped;
  }

  float dx = ver2.x *(ver1.w)/(ver2.w) - ver1.x;
  float dy = ver2.y *(ver1.w)/(ver2.w) - ver1.y;
  float dz = ver2.z *(ver1.w)/(ver2.w) - ver1.z;
  float eps = 1e-9;
  
  float p[] = {-dx, dx, -dy, dy};
  float q[] = {ver1.x + ver1.w, ver1.w - ver1.x, ver1.y + ver1.w, ver1.w - ver1.y};

  float u1 = 0.0f, u2 = 1.0f;
  for (int i = 0 ; i < 4 ; i++) {
    if (fabsf(p[i]) < eps) {
      clipped.out = true;
      if (q[i] < 0) return clipped;
    }
    else {
      float t = q[i] / p[i];
      if (p[i] < 0) u1 = std::fmax(u1, t);
      else u2 = std::fmin(u2, t);
    }
  }
  if (u1 > u2) { clipped.out = true; return clipped; }
  if (u1 < 0.0f && u2 > 1.0f) { clipped.out = false; return clipped; }
  float xc1 = (ver1.x + u1*dx)/ver1.w;
  float yc1 = (ver1.y + u1*dy)/ver1.w;
  float zc1 = (ver1.z + u1*dz)/ver1.w;
  float xc2 = (ver1.x + u2*dx)/ver1.w;
  float yc2 = (ver1.y + u2*dy)/ver1.w;
  float zc2 = (ver1.z + u2*dz)/ver1.w;
  clipped.v1 = Vec3(xc1, yc1, zc1);
  clipped.v2 = Vec3(xc2, yc2, zc2);
  clipped.out = false;
  clipped.status = true;
  return clipped;
}

std::vector<Vec3> boundFindXZ (Vec3 v0, Vec3 v1, Vec3 v2, bool full) {
  std::vector<Vec3> corners;
  for (int i = -1 ; i < 2 ; i+=2) {
    for (int j = -1 ; j < 2; j+=2) {
      float area = edgeFunc(v0, v1, v2);
      float w0 = edgeFunc(Vec3((float)i, (float)j, 1.0f), v1, v2)/area;
      float w1 = edgeFunc(Vec3((float)i, (float)j, 1.0f), v2, v0)/area;
      float w2 = edgeFunc(Vec3((float)i, (float)j, 1.0f), v0, v1)/area;
      float z = (w0*(v0.z) + w1*(v1.z) + w2*(v2.z));
      bool x = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
      if (x) {
        corners.push_back(Vec3((float)i, (float)j, z));
        if (!full) return corners;
      }
    }
  }
  return corners;
}

void clipTriangle_xz (const Vec4& v0, const Vec4& v1, const Vec4& v2, const Vec3 color, const int modelInd, const int triInd) {
  lineClip1 clip1 = liang_barskyClip_xz(v0, v1);
  lineClip1 clip2 = liang_barskyClip_xz(v1, v2);
  lineClip1 clip3 = liang_barskyClip_xz(v2, v0);

  Vec3 v0_ndc = {v0.x/v0.w, v0.y/v0.w, v0.z/v0.w};
  Vec3 v1_ndc = {v1.x/v1.w, v1.y/v1.w, v1.z/v1.w};
  Vec3 v2_ndc = {v2.x/v2.w, v2.y/v2.w, v2.z/v2.w};

  int check = (int)clip1.status + (int)clip2.status + (int)clip3.status;

  if (check == 0) {
    if (!clip1.out) {
      VOA_temp2.push_back(VBuff(v0_ndc, v1_ndc, v2_ndc, color, modelInd, triInd));
    }
    else {
      float w0 = edgeFunc(Vec3(1.0f, 1.0f, 1.0f), v1_ndc, v2_ndc);
      float w1 = edgeFunc(Vec3(1.0f, 1.0f, 1.0f), v2_ndc, v0_ndc);
      float w2 = edgeFunc(Vec3(1.0f, 1.0f, 1.0f), v0_ndc, v1_ndc);
      bool x = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
      if (x) {
        std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, true);
        if (corner.size() != 0) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], corner[2], color, modelInd, triInd));
          VOA_temp2.push_back(VBuff(corner[3], corner[1], corner[2], color, modelInd, triInd));
        }
      }
    }
  }
  if (check == 1) {
    std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, true);
    if (corner.size() == 1) {
      if (clip1.status) VOA_temp2.push_back(VBuff(clip1.v1, clip1.v2, corner[0], color, modelInd, triInd));
      if (clip2.status) VOA_temp2.push_back(VBuff(clip2.v1, clip2.v2, corner[0], color, modelInd, triInd));
      if (clip3.status) VOA_temp2.push_back(VBuff(clip3.v1, clip3.v2, corner[0], color, modelInd, triInd));
    }
    else if (corner.size() == 2) {
      if (clip1.status) {
        if (corner[0].x == corner[1].x) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], clip1.v1, color, modelInd, triInd));
          if (corner[0].y == clip1.v2.y) VOA_temp2.push_back(VBuff(corner[0], clip1.v2, clip1.v1, color, modelInd, triInd));
          else VOA_temp2.push_back(VBuff(corner[1], clip1.v1, clip1.v2, color, modelInd, triInd));
        }
        if (corner[0].y == corner[1].y) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], clip1.v1, color, modelInd, triInd));
          if (corner[0].x == clip1.v2.x) VOA_temp2.push_back(VBuff(corner[0], clip1.v2, clip1.v1, color, modelInd, triInd));
          else VOA_temp2.push_back(VBuff(corner[1], clip1.v1, clip1.v2, color, modelInd, triInd));
        }
      }
      if (clip2.status) {
        if (corner[0].x == corner[1].x) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], clip2.v1, color, modelInd, triInd));
          if (corner[0].y == clip2.v2.y) VOA_temp2.push_back(VBuff(corner[0], clip2.v2, clip2.v1, color, modelInd, triInd));
          else VOA_temp2.push_back(VBuff(corner[1], clip2.v1, clip2.v2, color, modelInd, triInd));
        }
        if (corner[0].y == corner[1].y) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], clip2.v1, color, modelInd, triInd));
          if (corner[0].x == clip2.v2.x) VOA_temp2.push_back(VBuff(corner[0], clip2.v2, clip2.v1, color, modelInd, triInd));
          else VOA_temp2.push_back(VBuff(corner[1], clip2.v1, clip2.v2, color, modelInd, triInd));
        }
      }
      if (clip3.status) {
        if (corner[0].x == corner[1].x) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], clip3.v1, color, modelInd, triInd));
          if (corner[0].y == clip3.v2.y) VOA_temp2.push_back(VBuff(corner[0], clip3.v2, clip3.v1, color, modelInd, triInd));
          else VOA_temp2.push_back(VBuff(corner[1], clip3.v1, clip3.v2, color, modelInd, triInd));
        }
        if (corner[0].y == corner[1].y) {
          VOA_temp2.push_back(VBuff(corner[0], corner[1], clip3.v1, color, modelInd, triInd));
          if (corner[0].x == clip3.v2.x) VOA_temp2.push_back(VBuff(corner[0], clip3.v2, clip3.v1, color, modelInd, triInd));
          else VOA_temp2.push_back(VBuff(corner[1], clip3.v1, clip3.v2, color, modelInd, triInd));
        }
      }
    }
  }
  if (check == 2) {
    if (!clip1.status) {
      if (clip1.out) {
        if (floatEq(clip2.v1.x, clip3.v2.x) || floatEq(clip2.v1.y, clip3.v2.y))
          VOA_temp2.push_back(VBuff(v2_ndc, clip2.v1, clip3.v2, color, modelInd, triInd));
        else {
          VOA_temp2.push_back(VBuff(v2_ndc, clip2.v1, clip3.v2, color, modelInd, triInd));
          std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) VOA_temp2.push_back(VBuff(corner[0], clip2.v1, clip3.v2, color, modelInd, triInd));
        }
      }
      else {
        if (floatEq(clip2.v2.x, clip3.v1.x) || floatEq(clip2.v2.y, clip3.v1.y)) {
          VOA_temp2.push_back(VBuff(v0_ndc, v1_ndc, clip3.v1, color, modelInd, triInd));
          VOA_temp2.push_back(VBuff(v1_ndc, clip3.v1, clip2.v2, color, modelInd, triInd));
        }
        else {
          std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) {
            VOA_temp2.push_back(VBuff(v0_ndc, v1_ndc, corner[0], color, modelInd, triInd));
            VOA_temp2.push_back(VBuff(v0_ndc, clip3.v1, corner[0], color, modelInd, triInd));
            VOA_temp2.push_back(VBuff(clip2.v2, v1_ndc, corner[0], color, modelInd, triInd));
          }
        }
      }
    }
    if (!clip2.status) {
      if (clip2.out) {
        if (floatEq(clip1.v2.x, clip3.v1.x) || floatEq(clip1.v2.y, clip3.v1.y))
          VOA_temp2.push_back(VBuff(v0_ndc, clip1.v2, clip3.v1, color, modelInd, triInd));
        else {
          VOA_temp2.push_back(VBuff(v0_ndc, clip1.v2, clip3.v1, color, modelInd, triInd));
          std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) VOA_temp2.push_back(VBuff(corner[0], clip1.v2, clip3.v1, color, modelInd, triInd));
        }
      }
      else {
        if (floatEq(clip1.v1.x, clip3.v2.x) || floatEq(clip1.v1.y, clip3.v2.y)) {
          VOA_temp2.push_back(VBuff(v1_ndc, v2_ndc, clip1.v1, color, modelInd, triInd));
          VOA_temp2.push_back(VBuff(v2_ndc, clip1.v1, clip3.v2, color, modelInd, triInd));
        }
        else {
          std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) {
            VOA_temp2.push_back(VBuff(v1_ndc, v2_ndc, corner[0], color, modelInd, triInd));
            VOA_temp2.push_back(VBuff(v1_ndc, clip1.v1, corner[0], color, modelInd, triInd));
            VOA_temp2.push_back(VBuff(clip3.v2, v2_ndc, corner[0], color, modelInd, triInd));
          }
        }
      }
    }
    if (!clip3.status) {
      if (clip3.out) {
        if (floatEq(clip2.v2.x, clip1.v1.x) || floatEq(clip2.v2.y, clip1.v1.y))
          VOA_temp2.push_back(VBuff(v1_ndc, clip2.v2, clip1.v1, color, modelInd, triInd));
        else {
          VOA_temp2.push_back(VBuff(v1_ndc, clip2.v2, clip1.v1, color, modelInd, triInd));
          std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) VOA_temp2.push_back(VBuff(corner[0], clip2.v2, clip1.v1, color, modelInd, triInd));
        }
      }
      else {
        if (floatEq(clip2.v1.x, clip1.v2.x) || floatEq(clip2.v1.y, clip1.v2.y)) {
          VOA_temp2.push_back(VBuff(v0_ndc, v2_ndc, clip1.v2, color, modelInd, triInd));
          VOA_temp2.push_back(VBuff(v2_ndc, clip1.v2, clip2.v1, color, modelInd, triInd));
        }
        else {
          std::vector<Vec3> corner = boundFindXZ(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) {
            VOA_temp2.push_back(VBuff(v0_ndc, v2_ndc, corner[0], color, modelInd, triInd));
            VOA_temp2.push_back(VBuff(v0_ndc, clip1.v2, corner[0], color, modelInd, triInd));
            VOA_temp2.push_back(VBuff(clip2.v1, v2_ndc, corner[0], color, modelInd, triInd));
          }
        }
      }
    }
  }
  if (check == 3) {
    bool v0_in = (fabsf(v0_ndc.x) <= 1 && fabsf(v0_ndc.y) <= 1);
    bool v1_in = (fabsf(v1_ndc.x) <= 1 && fabsf(v1_ndc.y) <= 1);
    bool v2_in = (fabsf(v2_ndc.x) <= 1 && fabsf(v2_ndc.y) <= 1);
    if (v0_in) {
      VOA_temp2.push_back(VBuff(v0_ndc, clip2.v1, clip2.v2, color, modelInd, triInd));
      VOA_temp2.push_back(VBuff(v0_ndc, clip1.v2, clip2.v1, color, modelInd, triInd));
      VOA_temp2.push_back(VBuff(v0_ndc, clip3.v1, clip2.v2, color, modelInd, triInd));
    }
    if (v1_in) {
      VOA_temp2.push_back(VBuff(v1_ndc, clip3.v1, clip3.v2, color, modelInd, triInd));
      VOA_temp2.push_back(VBuff(v1_ndc, clip1.v1, clip3.v2, color, modelInd, triInd));
      VOA_temp2.push_back(VBuff(v1_ndc, clip2.v2, clip3.v1, color, modelInd, triInd));
    }
    if (v2_in) {
      VOA_temp2.push_back(VBuff(v2_ndc, clip1.v1, clip1.v2, color, modelInd, triInd));
      VOA_temp2.push_back(VBuff(v2_ndc, clip3.v2, clip1.v1, color, modelInd, triInd));
      VOA_temp2.push_back(VBuff(v2_ndc, clip2.v1, clip1.v2, color, modelInd, triInd));
    }
  }
}

void zClip(int bypass) {
  if (bypass == 0) {
    for (VBuff1& v : VOA_temp) {
      clipTriangle_xz(Vec4(v.v0.x, v.v0.z, v.v0.y, v.v0.w), Vec4(v.v1.x, v.v1.z, v.v1.y, v.v1.w), Vec4(v.v2.x, v.v2.z, v.v2.y, v.v2.w), v.color, v.modelInd, v.triInd);
    }
  }
  else if (bypass == 1) {
    for (VBuff1& v : VOA_temp) {
      VOA_temp2.push_back(VBuff(Vec3(v.v0.x/v.v0.w, v.v0.z/v.v0.w, v.v0.y/v.v0.w), Vec3(v.v1.x/v.v1.w, v.v1.z/v.v1.w, v.v1.y/v.v1.w), Vec3(v.v2.x/v.v2.w, v.v2.z/v.v2.w, v.v2.y/v.v2.w), v.color, v.modelInd, v.triInd));
    }
  }
}

void VertexBuffer_update() {
  for (VBuff& v : VOA_temp2)
    VOA.push_back(VBuff(Vec3(v.v0.x, v.v0.z, v.v0.y), Vec3(v.v1.x, v.v1.z, v.v1.y), Vec3(v.v2.x, v.v2.z, v.v2.y), v.color, v.modelInd, v.triInd));
}
