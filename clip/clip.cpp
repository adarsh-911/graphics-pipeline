#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"
#include "clip.h"

std::vector<VBuff1> VOA_temp;

bool floatEq(float x, float y) {
  float eps = 1e-5;
  if (std::fabs(x-y) < eps) return true;
  else return false;
}

lineClip liang_barskyClip (const glm::vec4 ver1, const glm::vec4 ver2) {
  lineClip clipped;
  clipped.v1 = Vec4(ver1.x, ver1.y, ver1.z, ver1.w);
  clipped.v2 = Vec4(ver2.x, ver2.y, ver2.z, ver2.w);
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
  float xc1 = (ver1.x + u1*dx);
  float yc1 = (ver1.y + u1*dy);
  float zc1 = (ver1.z + u1*dz);
  float xc2 = (ver1.x + u2*dx);
  float yc2 = (ver1.y + u2*dy);
  float zc2 = (ver1.z + u2*dz);
  clipped.v1 = Vec4(xc1, yc1, zc1, ver1.w);
  clipped.v2 = Vec4(xc2, yc2, zc2, ver1.w);
  clipped.out = false;
  clipped.status = true;
  return clipped;
}

float edgeFunc(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
  return (v1.x - v0.x)*(v2.y - v0.y) - (v2.x - v0.x)*(v1.y - v0.y);
}

std::vector<Vec4> boundFind (Vec4 v0, Vec4 v1, Vec4 v2, bool full) {
  std::vector<Vec4> corners;
  for (int i = -1 ; i < 2 ; i+=2) {
    for (int j = -1 ; j < 2; j+=2) {
      float area = edgeFunc(Vec3(v0.x/v0.w, v0.y/v0.w, v0.z/v0.w), Vec3(v1.x/v1.w, v1.y/v1.w, v1.z/v1.w), Vec3(v2.x/v2.w, v2.y/v2.w, v2.z/v2.w));
      float w0 = edgeFunc(Vec3((float)i, (float)j, 1.0f), Vec3(v1.x/v1.w, v1.y/v1.w, v1.z/v1.w), Vec3(v2.x/v2.w, v2.y/v2.w, v2.z/v2.w))/area;
      float w1 = edgeFunc(Vec3((float)i, (float)j, 1.0f), Vec3(v2.x/v2.w, v2.y/v2.w, v2.z/v2.w), Vec3(v0.x/v0.w, v0.y/v0.w, v0.z/v0.w))/area;
      float w2 = edgeFunc(Vec3((float)i, (float)j, 1.0f), Vec3(v0.x/v0.w, v0.y/v0.w, v0.z/v0.w), Vec3(v1.x/v1.w, v1.y/v1.w, v1.z/v1.w))/area;
      
      bool x = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
      if (x) {
        //float w = 1 / (w0*(1/v0.w) + w1*(1/v1.w) + w2*(1/v2.w));
        float w = (w0*(v0.w) + w1*(v1.w) + w2*(v2.w));
        float z = w / (w0*(v0.w/v0.z) + w1*(v1.w/v1.z) + w2*(v2.w/v2.z));
        corners.push_back(Vec4((float)i*w, (float)j*w, z, w));
        if (!full) return corners;
      }
    }
  }
  return corners;
}

void clipTriangle (const glm::vec4 v0, const glm::vec4 v1, const glm::vec4 v2, const Vec3 color, const int modelInd, const int triInd) {
  lineClip clip1 = liang_barskyClip(v0, v1);
  lineClip clip2 = liang_barskyClip(v1, v2);
  lineClip clip3 = liang_barskyClip(v2, v0);

  Vec4 v0_ndc = {v0.x, v0.y, v0.z, v0.w};
  Vec4 v1_ndc = {v1.x, v1.y, v1.z, v1.w};
  Vec4 v2_ndc = {v2.x, v2.y, v2.z, v2.w};

  int check = (int)clip1.status + (int)clip2.status + (int)clip3.status;

  if (check == 0) {
    if (!clip1.out) {
      VOA_temp.push_back(VBuff1(v0_ndc, v1_ndc, v2_ndc, color, modelInd, triInd));
    }
    else {
      float w0 = edgeFunc(Vec3(1.0f, 1.0f, 1.0f), Vec3(v1_ndc.x/v1_ndc.w, v1_ndc.y/v1_ndc.w, v1_ndc.z/v1_ndc.w), Vec3(v2_ndc.x/v2_ndc.w, v2_ndc.y/v2_ndc.w, v2_ndc.z/v2_ndc.w));
      float w1 = edgeFunc(Vec3(1.0f, 1.0f, 1.0f), Vec3(v2_ndc.x/v2_ndc.w, v2_ndc.y/v2_ndc.w, v2_ndc.z/v2_ndc.w), Vec3(v0_ndc.x/v0_ndc.w, v0_ndc.y/v0_ndc.w, v0_ndc.z/v0_ndc.w));
      float w2 = edgeFunc(Vec3(1.0f, 1.0f, 1.0f), Vec3(v0_ndc.x/v0_ndc.w, v0_ndc.y/v0_ndc.w, v0_ndc.z/v0_ndc.w), Vec3(v1_ndc.x/v1_ndc.w, v1_ndc.y/v1_ndc.w, v1_ndc.z/v1_ndc.w));
      bool x = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
      if (x) {
        std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, true);
        if (corner.size() != 0) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], corner[2], color, modelInd, triInd));
          VOA_temp.push_back(VBuff1(corner[3], corner[1], corner[2], color, modelInd, triInd));
        }
      }
    }
  }
  if (check == 1) {
    std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, true);
    if (corner.size() == 1) {
      if (clip1.status) VOA_temp.push_back(VBuff1(clip1.v1, clip1.v2, corner[0], color, modelInd, triInd));
      if (clip2.status) VOA_temp.push_back(VBuff1(clip2.v1, clip2.v2, corner[0], color, modelInd, triInd));
      if (clip3.status) VOA_temp.push_back(VBuff1(clip3.v1, clip3.v2, corner[0], color, modelInd, triInd));
    }
    else if (corner.size() == 2) {
      if (clip1.status) {
        if (corner[0].x == corner[1].x) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], clip1.v1, color, modelInd, triInd));
          if (corner[0].y == clip1.v2.y) VOA_temp.push_back(VBuff1(corner[0], clip1.v2, clip1.v1, color, modelInd, triInd));
          else VOA_temp.push_back(VBuff1(corner[1], clip1.v1, clip1.v2, color, modelInd, triInd));
        }
        if (corner[0].y == corner[1].y) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], clip1.v1, color, modelInd, triInd));
          if (corner[0].x == clip1.v2.x) VOA_temp.push_back(VBuff1(corner[0], clip1.v2, clip1.v1, color, modelInd, triInd));
          else VOA_temp.push_back(VBuff1(corner[1], clip1.v1, clip1.v2, color, modelInd, triInd));
        }
      }
      if (clip2.status) {
        if (corner[0].x == corner[1].x) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], clip2.v1, color, modelInd, triInd));
          if (corner[0].y == clip2.v2.y) VOA_temp.push_back(VBuff1(corner[0], clip2.v2, clip2.v1, color, modelInd, triInd));
          else VOA_temp.push_back(VBuff1(corner[1], clip2.v1, clip2.v2, color, modelInd, triInd));
        }
        if (corner[0].y == corner[1].y) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], clip2.v1, color, modelInd, triInd));
          if (corner[0].x == clip2.v2.x) VOA_temp.push_back(VBuff1(corner[0], clip2.v2, clip2.v1, color, modelInd, triInd));
          else VOA_temp.push_back(VBuff1(corner[1], clip2.v1, clip2.v2, color, modelInd, triInd));
        }
      }
      if (clip3.status) {
        if (corner[0].x == corner[1].x) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], clip3.v1, color, modelInd, triInd));
          if (corner[0].y == clip3.v2.y) VOA_temp.push_back(VBuff1(corner[0], clip3.v2, clip3.v1, color, modelInd, triInd));
          else VOA_temp.push_back(VBuff1(corner[1], clip3.v1, clip3.v2, color, modelInd, triInd));
        }
        if (corner[0].y == corner[1].y) {
          VOA_temp.push_back(VBuff1(corner[0], corner[1], clip3.v1, color, modelInd, triInd));
          if (corner[0].x == clip3.v2.x) VOA_temp.push_back(VBuff1(corner[0], clip3.v2, clip3.v1, color, modelInd, triInd));
          else VOA_temp.push_back(VBuff1(corner[1], clip3.v1, clip3.v2, color, modelInd, triInd));
        }
      }
    }
  }
  if (check == 2) {
    if (!clip1.status) {
      if (clip1.out) {
        if (floatEq(clip2.v1.x/clip2.v1.w, clip3.v2.x/clip3.v2.w) || floatEq(clip2.v1.y/clip2.v1.w, clip3.v2.y/clip3.v2.w))
          VOA_temp.push_back(VBuff1(v2_ndc, clip2.v1, clip3.v2, color, modelInd, triInd));
        else {
          VOA_temp.push_back(VBuff1(v2_ndc, clip2.v1, clip3.v2, color, modelInd, triInd));
          std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) VOA_temp.push_back(VBuff1(corner[0], clip2.v1, clip3.v2, color, modelInd, triInd));
        }
      }
      else {
        if (floatEq(clip2.v2.x/clip2.v2.w, clip3.v1.x/clip3.v1.w) || floatEq(clip2.v2.y/clip2.v2.w, clip3.v1.y/clip3.v1.w)) {
          VOA_temp.push_back(VBuff1(v0_ndc, v1_ndc, clip3.v1, color, modelInd, triInd));
          VOA_temp.push_back(VBuff1(v1_ndc, clip3.v1, clip2.v2, color, modelInd, triInd));
        }
        else {
          std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) {
            VOA_temp.push_back(VBuff1(v0_ndc, v1_ndc, corner[0], color, modelInd, triInd));
            VOA_temp.push_back(VBuff1(v0_ndc, clip3.v1, corner[0], color, modelInd, triInd));
            VOA_temp.push_back(VBuff1(clip2.v2, v1_ndc, corner[0], color, modelInd, triInd));
          }
        }
      }
    }
    if (!clip2.status) {
      if (clip2.out) {
        if (floatEq(clip1.v2.x/clip1.v2.w, clip3.v1.x/clip3.v1.w) || floatEq(clip1.v2.y/clip1.v2.w, clip3.v1.y/clip3.v1.w))
          VOA_temp.push_back(VBuff1(v0_ndc, clip1.v2, clip3.v1, color, modelInd, triInd));
        else {
          VOA_temp.push_back(VBuff1(v0_ndc, clip1.v2, clip3.v1, color, modelInd, triInd));
          std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) VOA_temp.push_back(VBuff1(corner[0], clip1.v2, clip3.v1, color, modelInd, triInd));
        }
      }
      else {
        if (floatEq(clip1.v1.x/clip1.v1.w, clip3.v2.x/clip3.v2.w) || floatEq(clip1.v1.y/clip1.v1.w, clip3.v2.y/clip3.v2.w)) {
          VOA_temp.push_back(VBuff1(v1_ndc, v2_ndc, clip1.v1, color, modelInd, triInd));
          VOA_temp.push_back(VBuff1(v2_ndc, clip1.v1, clip3.v2, color, modelInd, triInd));
        }
        else {
          std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) {
            VOA_temp.push_back(VBuff1(v1_ndc, v2_ndc, corner[0], color, modelInd, triInd));
            VOA_temp.push_back(VBuff1(v1_ndc, clip1.v1, corner[0], color, modelInd, triInd));
            VOA_temp.push_back(VBuff1(clip3.v2, v2_ndc, corner[0], color, modelInd, triInd));
          }
        }
      }
    }
    if (!clip3.status) {
      if (clip3.out) {
        if (floatEq(clip2.v2.x/clip2.v2.w, clip1.v1.x/clip1.v1.w) || floatEq(clip2.v2.y/clip2.v2.w, clip1.v1.y/clip1.v1.w))
          VOA_temp.push_back(VBuff1(v1_ndc, clip2.v2, clip1.v1, color, modelInd, triInd));
        else {
          VOA_temp.push_back(VBuff1(v1_ndc, clip2.v2, clip1.v1, color, modelInd, triInd));
          std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) VOA_temp.push_back(VBuff1(corner[0], clip2.v2, clip1.v1, color, modelInd, triInd));
        }
      }
      else {
        if (floatEq(clip2.v1.x/clip2.v1.w, clip1.v2.x/clip1.v2.w) || floatEq(clip2.v1.y/clip2.v1.w, clip1.v2.y/clip1.v2.w)) {
          VOA_temp.push_back(VBuff1(v0_ndc, v2_ndc, clip1.v2, color, modelInd, triInd));
          VOA_temp.push_back(VBuff1(v2_ndc, clip1.v2, clip2.v1, color, modelInd, triInd));
        }
        else {
          std::vector<Vec4> corner = boundFind(v0_ndc, v1_ndc, v2_ndc, false);
          if (corner.size() != 0) {
            VOA_temp.push_back(VBuff1(v0_ndc, v2_ndc, corner[0], color, modelInd, triInd));
            VOA_temp.push_back(VBuff1(v0_ndc, clip1.v2, corner[0], color, modelInd, triInd));
            VOA_temp.push_back(VBuff1(clip2.v1, v2_ndc, corner[0], color, modelInd, triInd));
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
      VOA_temp.push_back(VBuff1(v0_ndc, clip2.v1, clip2.v2, color, modelInd, triInd));
      VOA_temp.push_back(VBuff1(v0_ndc, clip1.v2, clip2.v1, color, modelInd, triInd));
      VOA_temp.push_back(VBuff1(v0_ndc, clip3.v1, clip2.v2, color, modelInd, triInd));
    }
    if (v1_in) {
      VOA_temp.push_back(VBuff1(v1_ndc, clip3.v1, clip3.v2, color, modelInd, triInd));
      VOA_temp.push_back(VBuff1(v1_ndc, clip1.v1, clip3.v2, color, modelInd, triInd));
      VOA_temp.push_back(VBuff1(v1_ndc, clip2.v2, clip3.v1, color, modelInd, triInd));
    }
    if (v2_in) {
      VOA_temp.push_back(VBuff1(v2_ndc, clip1.v1, clip1.v2, color, modelInd, triInd));
      VOA_temp.push_back(VBuff1(v2_ndc, clip3.v2, clip1.v1, color, modelInd, triInd));
      VOA_temp.push_back(VBuff1(v2_ndc, clip2.v1, clip1.v2, color, modelInd, triInd));
    }
  }
}

void clip() {
  for (const modelClass& transModel : modelsCam) {
    for (const modelIdx& triIndModel : modelTriangleInd) {
      if (triIndModel.name == transModel.name) {
        int triangleInd = 0;
        for (const Ind& triInd : triIndModel.idx) {
          glm::vec4 ver1 = transModel.vertices[triInd.v0];
          glm::vec4 ver2 = transModel.vertices[triInd.v1];
          glm::vec4 ver3 = transModel.vertices[triInd.v2];
          clipTriangle(ver1, ver2, ver3, transModel.color, transModel.ind, triangleInd);
          triangleInd++;
        }
      }
    }
  }
}
