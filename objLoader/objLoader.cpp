#include "objLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>


bool OBJLoader::load(const std::string& filename) {
  this->vertices.clear();
  this->texCoords.clear();
  this->normals.clear();
  int id = 0;
  bool init = false;

  std::ifstream file(filename);
  if (!file.is_open()) {
      std::cerr << "Failed to open OBJ file: " << filename << std::endl;
      return false;
  }

  std::string line;

  while (std::getline(file, line)) {
      std::istringstream ss(line);
      std::string prefix;
      ss >> prefix;

      if (prefix == "v") {
          glm::vec3 pos;
          ss >> pos.x >> pos.y >> pos.z;
          this->vertices.push_back(pos);

      } else if (prefix == "vt") {
          glm::vec2 uv;
          ss >> uv.x >> uv.y;
          this->texCoords.push_back(uv);

      } else if (prefix == "vn") {
          glm::vec3 n;
          ss >> n.x >> n.y >> n.z;
          this->normals.push_back(n);

      } else if (prefix == "f") {
          idx idx, idx1, idx2;

          for (int i = 0; i < 3; ++i) {
              std::string vert;
              ss >> vert;
              int vi = 0, ti = 0, ni = 0;

              size_t firstSlash = vert.find('/');
              size_t secondSlash = vert.find('/', firstSlash + 1);

              if (firstSlash == std::string::npos) sscanf(vert.c_str(), "%d", &vi);
              else if (secondSlash == std::string::npos) sscanf(vert.c_str(), "%d/%d", &vi, &ti);
              else if (firstSlash + 1 == secondSlash) sscanf(vert.c_str(), "%d//%d", &vi, &ni);
              else sscanf(vert.c_str(), "%d/%d/%d", &vi, &ti, &ni);

              if (i == 0) {idx.a = vi - 1; idx1.a = ti - 1; idx2.a = ni - 1;}
              if (i == 1) {idx.b = vi - 1; idx1.b = ti - 1; idx2.b = ni - 1;}
              if (i == 2) {idx.c = vi - 1; idx1.c = ti - 1; idx2.c = ni - 1;}
          }

          this->pIdx.push_back(idx);
          this->tIdx.push_back(idx1);
          this->nIdx.push_back(idx2);
          this->texID.push_back(id);

      } else if (prefix == "g") {
          if (line != "g off") {
            if (init) id++;
            else init = true; 
          }
      }

  }
  return true;
}

const std::vector<glm::vec3>& OBJLoader::getVertices() const {
  return this->vertices;
}

const std::vector<glm::vec2>& OBJLoader::getTexCoords() const {
  return this->texCoords;
}

const std::vector<glm::vec3>& OBJLoader::getNormals() const {
  return this->normals;
}

const std::vector<idx>& OBJLoader::getIndices(int which) const {
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

const std::vector<int>& OBJLoader::getTexID() const {
  return this->texID;
}
