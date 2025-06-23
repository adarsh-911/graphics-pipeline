#pragma once

#include "../objLoader/objLoader.hpp"

class Model {
  public:
    std::string name;
    bool light;
    int texWidth, texHeight;

    glm::vec3 position;
    glm::vec3 axis;
    float angle;

    float scaleFactor;
    Color color;

    bool load(const std::string& filename);
    void loadTexture (const char* filename, int& width, int& height);

    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec2>& getTexCoords() const;
    std::vector<glm::vec3>& getNormals();

    const std::vector<idx>& getIndices(int which) const;
    const std::vector<Color>& getTexColors() const;
    std::vector<glm::vec3> normals;
  
  private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;

    std::vector<idx> pIdx;
    std::vector<idx> tIdx;
    std::vector<idx> nIdx;

    std::vector<Color> pixels;
};