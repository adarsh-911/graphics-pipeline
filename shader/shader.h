#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include "../objLoader/obj_loader.h"
#include "../texturing/texture.h"

namespace fs = std::filesystem;

struct modelClass {
  std::vector<glm::vec4> vertices;
  std::string name;
  Vec3 color;
  int ind;
};

struct modelTexClass {
  std::vector<Vec2uv> texcoords;
  std::string name;
};

struct modelNormalClass {
  std::vector<Vec3> normals;
  std::string name;
};

struct modelIdx {
  std::vector<Ind> idx;
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

struct Texture {
  std::vector<Color> pixels;
  int width;
  int height;
  std::string name;
};

struct Camera {
  glm::vec3 pos;
  glm::vec3 dir;
  Camera(glm::vec3 _pos, glm::vec3 _dir) : pos(_pos), dir(_dir) {}
};

struct Plane {
  glm::vec3 org;
  glm::vec3 norm;
};

struct Object {
  bool object, light;
  Object(bool _object = false, bool _light = false) : object(_object), light(_light) {}
};

int loadModels();
void generateWorld (const std::vector<modelClass> models);
void cameraInputs (Camera camera);

extern std::vector<modelClass> models;
extern std::vector<modelClass> modelsWorld;
extern std::vector<modelClass> modelsCam;
extern std::vector<modelIdx> modelTriangleInd;
extern std::vector<modelTexClass> modelTexCords;
extern std::vector<modelIdx> modelTexCordsInd;
extern std::vector<Texture> modelTexColors;
extern std::vector<modelNormalClass> modelNormals;
extern std::vector<modelIdx> modelNormInd;
extern std::vector<glm::vec3> lightSources;
extern Camera cameraParam;
extern glm::mat4 transform_matrix;