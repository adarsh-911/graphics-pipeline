#pragma once

#include "../modelLoader/model.hpp"
#include "../screen.hpp"

class Camera {
  public:
    glm::vec3 position;
    glm::vec3 direction;

    const float NEAR = 1.0f;
    const float FAR = 100.0f;
    const float fovy = glm::radians(110.f);
};

struct modelBuff {
  std::vector<glm::vec4> vertices;
  std::string name;
  Color color;
  int idx;
};

struct Plane {
  glm::vec3 org;
  glm::vec3 norm;
};

void generateWorld();
void cameraInputs (Camera camera);
extern std::vector<modelBuff> worldSpace;
extern std::vector<modelBuff> cameraSpace;
extern std::vector<glm::vec3> lightSources;

extern Camera cameraInst;
extern glm::mat4 WORLD_TO_SCREEN;