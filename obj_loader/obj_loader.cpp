#include "obj_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool OBJLoader::load(const std::string& filename) {
  triangles.clear();
  vertices.clear();

  std::ifstream file(filename);
  if (!file.is_open()) {
      std::cerr << "Failed to open OBJ file: " << filename << std::endl;
      return false;
  }

  std::vector<Vec3> positions;
  std::vector<Vec2uv> texcoords;
  std::vector<Vec3> normals;

  std::string line;
  while (std::getline(file, line)) {
      std::istringstream ss(line);
      std::string prefix;
      ss >> prefix;

      if (prefix == "v") {
          Vec3 pos;
          ss >> pos.x >> pos.y >> pos.z;
          vertices.push_back(Vec4(pos.x, pos.y, pos.z));
          positions.push_back(pos);
      } else if (prefix == "vt") {
          Vec2uv uv;
          ss >> uv.u >> uv.v;
          texcoords.push_back(uv);
      } else if (prefix == "vn") {
          Vec3 n;
          ss >> n.x >> n.y >> n.z;
          normals.push_back(n);
      } else if (prefix == "f") {
          Triangle tri;
          Ind idx;
          Ind idx1;
          for (int i = 0; i < 3; ++i) {
              std::string vert;
              ss >> vert;
              int vi = 0, ti = 0, ni = 0;
              size_t firstSlash = vert.find('/');
              size_t secondSlash = vert.find('/', firstSlash + 1);

              if (firstSlash == std::string::npos) {
                  // Format: v
                  sscanf(vert.c_str(), "%d", &vi);
              } else if (secondSlash == std::string::npos) {
                  // Format: v/t
                  sscanf(vert.c_str(), "%d/%d", &vi, &ti);
              } else if (firstSlash + 1 == secondSlash) {
                  // Format: v//n
                  sscanf(vert.c_str(), "%d//%d", &vi, &ni);
              } else {
                  // Format: v/t/n
                  sscanf(vert.c_str(), "%d/%d/%d", &vi, &ti, &ni);
              }
              //sscanf(vert.c_str(), "%d/%d/%d", &vi, &ti, &ni);

              Vertex v;
              if (vi > 0) v.position = positions[vi - 1];
              if (ti > 0) v.texcoord = texcoords[ti - 1];
              if (ni > 0) v.normal = normals[ni - 1];

              if (i == 0) {tri.v0 = v; idx.v0 = vi - 1; idx1.v0 = ti - 1;}
              if (i == 1) {tri.v1 = v; idx.v1 = vi - 1; idx1.v1 = ti - 1;}
              if (i == 2) {tri.v2 = v; idx.v2 = vi - 1; idx1.v2 = ti - 1;}
          }
          triangles.push_back(tri);
          triangleInd.push_back(idx);
          texInd.push_back(idx1);
      }
  }

  return true;
}

const std::vector<Triangle>& OBJLoader::getTriangles() const {
  return triangles;
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
