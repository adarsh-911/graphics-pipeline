#include "zClip.hpp"
#include "../modelLoader/loadModels.hpp"

std::vector<ZBuff> zclipSpace;

float Zmin, Zmax;

bool checkInside (const glm::vec4& v) {
  return (v.z <= Zmax && v.z >= Zmin);
}

zlineClip liang_barsky_z_plane (const glm::vec4& ver0, const glm::vec4& ver1) {
  zlineClip result;
  result.status = false;
  float t0 = 0.0f;
  float t1 = 1.0f;

  glm::vec3 p0(ver0);
  glm::vec3 p1(ver1);
  
  p1 *= ver0.w/ver1.w;
  glm::vec3 d = p1 - p0;

  if (near.org.z < far.org.z) {
    Zmin = near.org.z;
    Zmax = far.org.z;
  }
  else {
    Zmin = far.org.z;
    Zmax = near.org.z;
  }

  if (checkInside(ver0) && checkInside(ver1)) {
    result.out = false;
    return result;
  }
  
  {
  float p = -d.z;
  float q = p0.z - Zmin;

  if (p == 0) {
    if (q < 0) { result.out = true; return result; }
  } 
  else {
    float r = q / p;
    if (p < 0) t0 = std::max(t0, r);
    else t1 = std::min(t1, r);
  }
  } {
  float p = d.z;
  float q = Zmax - p0.z;

  if (p == 0) {
    if (q < 0) { result.out = true; return result; }
  } 
  else {
    float r = q / p;
    if (p < 0) t0 = std::max(t0, r);
    else t1 = std::min(t1, r);
  }
  }
  if (t0 > t1) { result.out = true; return result; }

  glm::vec3 start = p0 + t0 * d;
  glm::vec3 end = p0 + t1 * d;
  
  result.v1 = glm::vec4(start.x, start.y, start.z, ver0.w);
  result.v2 = glm::vec4(end.x, end.y, end.z, ver0.w);
  result.status = true;
  result.out = false;
  return result;
}

void clipTriangle_z_plane (const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, const Color color, const int modelIdx, const int triIdx){
  zlineClip clip1 = liang_barsky_z_plane(v0, v1);
  zlineClip clip2 = liang_barsky_z_plane(v1, v2);
  zlineClip clip3 = liang_barsky_z_plane(v2, v0);

  int check = (int)clip1.status + (int)clip2.status + (int)clip3.status;
  
  if (check == 0) {
    if (!clip1.out) zclipSpace.push_back(ZBuff(v0, v1, v2, color, modelIdx, triIdx));
  }
  
  if (check == 2) {
    if (!clip1.status) {
      if (!clip1.out) {
        zclipSpace.push_back(ZBuff(v0, v1, clip2.v2, color, modelIdx, triIdx));
        zclipSpace.push_back(ZBuff(v0, clip3.v1, clip2.v2, color, modelIdx, triIdx));
      }
      else {
        if (checkInside(v2)) { zclipSpace.push_back(ZBuff(v2, clip2.v1, clip3.v2, color, modelIdx, triIdx));}
        else {
          zclipSpace.push_back(ZBuff(clip2.v1, clip2.v2, clip3.v1, color, modelIdx, triIdx));
          zclipSpace.push_back(ZBuff(clip2.v1, clip3.v1, clip3.v2, color, modelIdx, triIdx));
        }
      }
    }
    if (!clip2.status) {
      if (!clip2.out) {
        zclipSpace.push_back(ZBuff(v1, v2, clip1.v1, color, modelIdx, triIdx));
        zclipSpace.push_back(ZBuff(v2, clip1.v1, clip3.v2, color, modelIdx, triIdx));
        //printv(v1, v2, clip1.v1);
        //printv(v2, clip1.v1, clip3.v2);
      }
      else {
        if (checkInside(v0)) zclipSpace.push_back(ZBuff(v0, clip1.v2, clip3.v1, color, modelIdx, triIdx));
        else {
          zclipSpace.push_back(ZBuff(clip1.v1, clip1.v2, clip3.v2, color, modelIdx, triIdx));
          zclipSpace.push_back(ZBuff(clip3.v1, clip3.v2, clip1.v2, color, modelIdx, triIdx));
        }
      }
    }
    if (!clip3.status) {
      if (!clip3.out) {
        zclipSpace.push_back(ZBuff(v2, v0, clip2.v1, color, modelIdx, triIdx));
        zclipSpace.push_back(ZBuff(v0, clip2.v1, clip1.v2, color, modelIdx, triIdx));
      }
      else {
        if (checkInside(v1)) zclipSpace.push_back(ZBuff(v1, clip1.v1, clip2.v2, color, modelIdx, triIdx));
        else {
          zclipSpace.push_back(ZBuff(clip1.v1, clip1.v2, clip2.v1, color, modelIdx, triIdx));
          zclipSpace.push_back(ZBuff(clip2.v1, clip2.v2, clip1.v1, color, modelIdx, triIdx));
        }
      }
    }
  }

  if (check == 3) {
    if (checkInside(v0)) {
      zclipSpace.push_back(ZBuff(v0, clip1.v2, clip3.v1, color, modelIdx, triIdx));
      zclipSpace.push_back(ZBuff(clip1.v2, clip3.v1, clip2.v1, color, modelIdx, triIdx));
      zclipSpace.push_back(ZBuff(clip2.v1, clip2.v2, clip3.v1, color, modelIdx, triIdx));
    }
    if (checkInside(v1)) {
      zclipSpace.push_back(ZBuff(v1, clip1.v1, clip2.v2, color, modelIdx, triIdx));
      zclipSpace.push_back(ZBuff(clip1.v1, clip2.v2, clip3.v2, color, modelIdx, triIdx));
      zclipSpace.push_back(ZBuff(clip3.v1, clip3.v2, clip2.v2, color, modelIdx, triIdx));
    }
    if (checkInside(v2)) {
      zclipSpace.push_back(ZBuff(v2, clip3.v2, clip2.v1, color, modelIdx, triIdx));
      zclipSpace.push_back(ZBuff(clip3.v2, clip2.v1, clip1.v1, color, modelIdx, triIdx));
      zclipSpace.push_back(ZBuff(clip1.v1, clip1.v2, clip2.v1, color, modelIdx, triIdx));
    }
  }

}

void zClip(bool bypass) {
  for (const modelBuff& transModel : cameraSpace) {
    for (const Model& model : models) {
      if (model.name == transModel.name) {
        int triangleIdx = 0;
        for (const idx& triIdx : model.getIndices(1)) {
          glm::vec4 ver1 = transModel.vertices[triIdx.a];
          glm::vec4 ver2 = transModel.vertices[triIdx.b];
          glm::vec4 ver3 = transModel.vertices[triIdx.c];
          if (!bypass) clipTriangle_z_plane(ver1, ver2, ver3, transModel.color, transModel.idx, triangleIdx);
          else zclipSpace.push_back(ZBuff(ver1, ver2, ver3, transModel.color, transModel.idx, triangleIdx));
          triangleIdx++;
        }
      }
    }
  }
}