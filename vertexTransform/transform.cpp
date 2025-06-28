#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "transform.hpp"

std::vector<Model> models;

std::vector<modelBuff> worldSpace;
std::vector<modelBuff> cameraSpace;
std::vector<modelBuff> screenSpace;
std::vector<glm::vec3> lightSources;

Camera cameraInst;
Plane near, far;

glm::mat4 WORLD_TO_SCREEN;
glm::mat4 WORLD_TO_CAM;
glm::mat4 CAM_TO_SCREEN;

template <typename T>
float signum(T val) {
    return (T(0) < val) - (val < T(0));
}

glm::vec4 normalizeModel(glm::vec4& v) {
  float maxVal = std::max({ std::abs(v.x), std::abs(v.y), std::abs(v.z) });
  if (maxVal != 0.0f) {
        v.x /= maxVal;
        v.y /= maxVal;
        v.z /= maxVal;
  }
  return v;
}

glm::vec4 transformToWorld (const glm::vec4& vertex, const glm::vec3 pos, const float angle, const glm::vec3 axis) {
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
  glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, axis);
  glm::vec4 obj_worldSpace = (translate * rotate) * vertex;

  return obj_worldSpace;
}

void transformNormals(Model& model) {
  int i = 0;
  glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), model.angle, model.axis);

  for (glm::vec3& normal : model.getNormals()) {
    glm::vec4 normUpdate = rotate*glm::vec4(normal.x, normal.y, normal.z, 1.0f);
    (model.normals)[i] = glm::vec3(normUpdate.x, normUpdate.y, normUpdate.z);
    i++;
  }
}

glm::vec4 transformToCamera (glm::vec4 obj_worldSpace, Camera camera) {

  glm::vec4 obj_cam = WORLD_TO_CAM * obj_worldSpace; 

  return obj_cam;
}

glm::vec4 transformToScreen (glm::vec4 obj_worldSpace, Camera camera) {

  glm::vec4 obj_proj = WORLD_TO_SCREEN * obj_worldSpace;

  return glm::vec4(obj_proj.x, obj_proj.y, obj_proj.z, obj_proj.w);
}

void generateModel (Model model) {
  modelBuff modelInst;
  glm::vec3 sum(0.0f);

  for (const glm::vec3& vertex : model.getVertices()) {
    glm::vec4 scaled_vertex(vertex.x*model.scaleFactor, vertex.y*model.scaleFactor, vertex.z*model.scaleFactor, 1.0f);
    glm::vec4 transVertex = transformToWorld(scaled_vertex, model.position, model.angle, model.axis);
    //transformNormals(model);
    modelInst.vertices.push_back(transVertex);
    if (model.light) sum += glm::vec3(transVertex.x, transVertex.y, transVertex.z);
  }
  modelInst.name = model.name;
  modelInst.color = model.color;
  modelInst.idx = worldSpace.size();
  worldSpace.push_back(modelInst);

  if (model.light) lightSources.push_back(sum/static_cast<float>(modelInst.vertices.size()));
}

void calculateClipPlanes(Camera camera) {
  glm::vec4 nearplane = glm::vec4(glm::translate(glm::mat4(1.0f), camera.direction * camera.NEAR) * glm::vec4(camera.position, 1.0f));
  near.org = WORLD_TO_CAM * nearplane;
  near.norm = camera.direction;

  glm::vec4 farplane = glm::vec4(glm::translate(glm::mat4(1.0f), camera.direction * camera.FAR) * glm::vec4(camera.position, 1.0f));
  far.org = WORLD_TO_CAM * farplane;
  far.norm = camera.direction;
}

void generateMatrix () {
  
  glm::mat4 VIEW_MAT = glm::lookAt(cameraInst.position, cameraInst.position + cameraInst.direction, glm::vec3(0.0f, 1.0f, 0.0f));
  //glm::mat4 TRANS_MAT = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraInst.position.x, -cameraInst.position.y, -cameraInst.position.z));
  glm::mat4 TRANS_MAT(1.0f);
  glm::mat4 PROJ_MAT = glm::perspective(cameraInst.fovy, (float)(Screen::WIDTH/Screen::HEIGHT), cameraInst.NEAR, cameraInst.FAR);

  WORLD_TO_CAM = TRANS_MAT * VIEW_MAT;
  CAM_TO_SCREEN = PROJ_MAT;
  WORLD_TO_SCREEN = PROJ_MAT * TRANS_MAT *  VIEW_MAT;
}

void cameraInputs (Camera camera) {
  cameraInst.position = camera.position;
  cameraInst.direction = camera.direction;

  generateMatrix();
  calculateClipPlanes(camera);
  
  for (const modelBuff& modelWorldspace : worldSpace) {
    modelBuff modelInst;
    modelBuff modelInst2;
    for (const glm::vec4& vertex : modelWorldspace.vertices) {
      glm::vec4 transVertexCam = transformToCamera(vertex, camera);
      glm::vec4 transVertex = transformToScreen(vertex, camera);
      modelInst.vertices.push_back(transVertex);
      modelInst2.vertices.push_back(transVertexCam);
    }
    modelInst.name = modelWorldspace.name;
    modelInst.color = modelWorldspace.color;
    modelInst.idx = modelWorldspace.idx;
    screenSpace.push_back(modelInst);

    modelInst2.name = modelWorldspace.name;
    modelInst2.color = modelWorldspace.color;
    modelInst2.idx = modelWorldspace.idx;
    cameraSpace.push_back(modelInst2);
  } 
}

void generateWorld() {
  for (Model& model : models) {
    generateModel(model);
  }
}

void readModels() {
  std::ifstream in("models.bin", std::ios::binary);
  if (!in) {
    std::cerr << "File not opened!\n";
  }
  size_t size;
  in.read(reinterpret_cast<char*>(&size), sizeof(size));
  models.resize(size);
  for (auto& model : models) {
    model.deserialize(in);
  }
}
