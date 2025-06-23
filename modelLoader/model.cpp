#define STB_IMAGE_IMPLEMENTATION
#include "model.hpp"
#include "stb_image.h"

bool Model::load(const std::string& filename) {
  OBJLoader loader;
  if (!loader.load(filename)) return false;
  else {
    this->vertices = loader.getVertices();
    this->texCoords = loader.getTexCoords();
    this->normals = loader.getNormals();
    this->pIdx = loader.getIndices(1);
    this->tIdx = loader.getIndices(2);
    this->nIdx = loader.getIndices(3);
  }
  return true;
}

void Model::loadTexture (const char* filename, int& width, int& height) {
  int channels;
  unsigned char* data = stbi_load(filename, &width, &height, &channels, 3); // force RGB
  if (!data) {
    std::cerr << "Failed to load image: " << filename << "\n";
    return;
  }
  this->pixels.reserve(width * height);
  for (int i = 0; i < width * height * 3; i += 3) {
    Color c = { data[i], data[i + 1], data[i + 2] };
    this->pixels.push_back(c);
  }
  stbi_image_free(data);
  
  return;
}

const std::vector<glm::vec3>& Model::getVertices() const {
  return this->vertices;
}

const std::vector<glm::vec2>& Model::getTexCoords() const {
  return this->texCoords;
}

std::vector<glm::vec3>& Model::getNormals() {
  return this->normals;
}

const std::vector<idx>& Model::getIndices(int which) const {
  static const std::vector<idx> empty;
  switch (which)
  {
  case 1:
    return this->pIdx; break;
  case 2:
    return this->tIdx; break;
  case 3:
    return this->nIdx; break; 
  default:
    return empty; break;
  }
}

const std::vector<Color>& Model::getTexColors() const {
  return this->pixels;
}
