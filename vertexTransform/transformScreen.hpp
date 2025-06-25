#pragma once

#include "transform.hpp"
#include "../clipping/zClip.hpp"

struct Vec4Less {
  bool operator()(const glm::vec4& a, const glm::vec4& b) const {
    for (int i = 0; i < 4; ++i) {
      if (a[i] < b[i]) return true;
      if (a[i] > b[i]) return false;
    }
    return false;
  }
};

struct Vec4Hash {
    std::size_t operator()(const glm::vec4& v) const {
        std::size_t h = 0;
        for (int i = 0; i < 4; ++i) {
            h ^= std::hash<float>{}(v[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

// Equality comparator (optional, GLM already has == operator)
struct Vec4Equal {
    bool operator()(const glm::vec4& a, const glm::vec4& b) const {
        return glm::all(glm::equal(a, b));
    }
};

void transformToScreen();
extern std::vector<ZBuff> screenSpace;