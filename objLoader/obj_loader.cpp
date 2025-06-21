#include "obj_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool OBJLoader::load(const std::string& filename) {
  vertices.clear();

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
          Vec3 pos;
          ss >> pos.x >> pos.y >> pos.z;
          vertices.push_back(Vec4(pos.x, pos.y, pos.z));
      } else if (prefix == "vt") {
          Vec2uv uv;
          ss >> uv.u >> uv.v;
          texcoords.push_back(uv);
      } else if (prefix == "vn") {
          Vec3 n;
          ss >> n.x >> n.y >> n.z;
          normals.push_back(n);
      } else if (prefix == "f") {
          Ind idx, idx1, idx2;
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

              if (i == 0) {idx.v0 = vi - 1; idx1.v0 = ti - 1; idx2.v0 = ni - 1;}
              if (i == 1) {idx.v1 = vi - 1; idx1.v1 = ti - 1; idx2.v1 = ni - 1;}
              if (i == 2) {idx.v2 = vi - 1; idx1.v2 = ti - 1; idx2.v2 = ni - 1;}
          }
          triangleInd.push_back(idx);
          texInd.push_back(idx1);
          normalInd.push_back(idx2);
      }
  }
  return true;
}

std::vector<Ind>& OBJLoader::getTriangleInd() {
  return triangleInd;
}

std::vector<Ind>& OBJLoader::getTexInd() {
  return texInd;
}

std::vector<Vec4>& OBJLoader::getVertices() {
  return vertices;
}

std::vector<Vec2uv>& OBJLoader::getTexCords() {
  return texcoords;
}

std::vector<Vec3>& OBJLoader::getNormals() {
  return normals;
}

std::vector<Ind>& OBJLoader::getNormalInd() {
  return normalInd;
}
