#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include "../obj_loader/obj_loader.h"

namespace fs = std::filesystem;

struct modelClass {
  std::vector<glm::vec4> vertices;
  std::string name;
  Vec3 color;
};

struct modelTriIndClass {
  std::vector<Ind> triangleInds;
  std::string name;
};

struct modelTexClass {
  std::vector<Vec2uv> texcoords;
  std::string name;
};

struct modelTexIndClass {
  std::vector<Ind> texInds;
  std::string name;
};

struct texClass {
  std::vector<Vec3> bary;
  std::string name;
};

struct modelWorld {
  glm::vec3 position;
  float angle;
  glm::vec3 axis;
};

struct Cam {
  glm::vec3 pos;
  glm::vec3 dir;
  Cam(glm::vec3 _pos, glm::vec3 _dir) : pos(_pos), dir(_dir) {}
};

struct Plane {
  glm::vec3 org;
  glm::vec3 norm;
};

int loadModels();
void generateWorld (const std::vector<modelClass> models);
void cameraInputs (Cam camera);

extern std::vector<modelClass> models;
extern std::vector<modelClass> modelsWorld;
extern std::vector<modelClass> modelsCam;
extern std::vector<modelTriIndClass> modelTriangleInd;
extern std::vector<modelTexClass> modelTexCords;
extern std::vector<modelTexIndClass> modelTexCordsInd;