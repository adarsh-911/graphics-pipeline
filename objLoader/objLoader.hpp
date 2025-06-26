#pragma once

#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <vector>

struct idx {
  int a, b, c;
};

struct Color {
  unsigned char r, g, b;
};

class OBJLoader {
  public:
    bool load(const std::string& filename);
    
    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec2>& getTexCoords() const;
    const std::vector<glm::vec3>& getNormals() const;

    const std::vector<idx>& getIndices(int which) const;
    const std::vector<int>& getTexID() const;

  private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    std::vector<idx> pIdx;
    std::vector<idx> tIdx;
    std::vector<idx> nIdx;
    std::vector<int> texID;
};