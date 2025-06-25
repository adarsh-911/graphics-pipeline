#include "transformScreen.hpp"

std::vector<ZBuff> screenSpace;

void transformToScreen() {
  std::unordered_map<glm::vec4, glm::vec4, Vec4Hash, Vec4Equal> transform;

  for (ZBuff& v : zclipSpace) {

    ZBuff vTrans;
    vTrans.color = v.color;
    vTrans.modelIdx = v.modelIdx;
    vTrans.triIdx = v.triIdx;

    if (glm::all(glm::equal(transform[v.v0], glm::vec4(0.0f)))) {
      vTrans.v0 = WORLD_TO_SCREEN * v.v0;
      transform[v.v0] = vTrans.v0;
    }
    else {
      vTrans.v0 = transform[v.v0];
    }

    if (glm::all(glm::equal(transform[v.v1], glm::vec4(0.0f)))) {
      vTrans.v1 = WORLD_TO_SCREEN * v.v1;
      transform[v.v1] = vTrans.v1;
    }
    else {
      vTrans.v1 = transform[v.v1];
    }
    
    if (glm::all(glm::equal(transform[v.v2], glm::vec4(0.0f)))) {
      vTrans.v2 = WORLD_TO_SCREEN * v.v2;
      transform[v.v2] = vTrans.v2;
    }
    else {
      vTrans.v2 = transform[v.v2];
    }

    screenSpace.push_back(vTrans);
  }

}