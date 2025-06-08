#include <bits/stdc++.h>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../obj_loader/obj_loader.h"
#include "shader.h"

std::vector<modelClass> models;
std::vector<modelClass> modelsWorld;
std::vector<modelClass> modelsCam;
std::vector<modelTriIndClass> modelTriangleInd;
std::vector<modelTexClass> modelTexCords;
std::vector<modelTexIndClass> modelTexCordsInd;

const int WIDTH = 600; //400
const int HEIGHT = 600; //240

const float NEAR = 1.0f;
const float FAR = 100.0f;

const float fovy = glm::radians(110.0f);

glm::vec4 normalizeModel(Vec4& v) {
  float maxVal = std::max({ std::abs(v.x), std::abs(v.y), std::abs(v.z) });
  if (maxVal != 0.0f) {
        v.x /= maxVal;
        v.y /= maxVal;
        v.z /= maxVal;
  }
  return glm::vec4(v.x, v.y, v.z, v.w);
}

int loadModels() {

  std::string pwd = std::string(fs::current_path());
  std::string models_path = pwd + "/models";

  for (const auto& entry : fs::recursive_directory_iterator(models_path)) {
    if (entry.is_regular_file() && entry.path().extension() == ".obj") {
      OBJLoader loader;
      if (!loader.load(entry.path())) return 1;
      else {
        modelClass m;
        modelTriIndClass mInd;
        modelTexClass mTex;
        modelTexIndClass mTxInd;

        for (Vec4& vertex : loader.getVertices()) m.vertices.push_back(normalizeModel(vertex));

        mInd.triangleInds = loader.getTriangleInd();
        mTex.texcoords = loader.getTexCords();
        mTxInd.texInds = loader.getTexInd();
        m.name = entry.path().stem().string();
        mInd.name = entry.path().stem().string();
        mTex.name = entry.path().stem().string();
        mTxInd.name = entry.path().stem().string();
        
        models.push_back(m);
        modelTriangleInd.push_back(mInd);
        modelTexCords.push_back(mTex);
        modelTexCordsInd.push_back(mTxInd);
      }
    }
  }
  return 0;
}

glm::vec4 transformVertexWorld (const glm::vec4& vertex, const modelWorld modelParam) {
  //Cam camera = {
  //  {0.0f, 0.0f, 0.0f}, //Position
  //  {0.0f, 0.0f, -1.0f}}; //Direction

  glm::mat4 trans = glm::translate(glm::mat4(1.0f), modelParam.position);
  glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), modelParam.angle, modelParam.axis);
  glm::vec4 obj_world = (trans * rotate) * vertex;

  return obj_world;
}

glm::vec4 transformToScreen (glm::vec4 obj_world, Cam camera) {
  Plane near, far;
  near.org = glm::vec3(glm::translate(glm::mat4(1.0f), camera.dir * NEAR) * glm::vec4(camera.pos, 1.0f));
  near.norm = camera.dir;

  far.org = glm::vec3(glm::translate(glm::mat4(1.0f), camera.dir * FAR) * glm::vec4(camera.pos, 1.0f));
  far.norm = camera.dir;

  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(camera.pos.x, -camera.pos.y, -camera.pos.z));  
  glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), -float(acos(-camera.dir.z/sqrt(camera.dir.z*camera.dir.z + camera.dir.x*camera.dir.x))), {0.0f, -1.0f, 0.0f});
  glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), -float(acos(-camera.dir.z/sqrt(camera.dir.z*camera.dir.z + camera.dir.y*camera.dir.y))), {1.0f, 0.0f, 0.0f});
  glm::vec4 obj_cam = (trans * rotateY * rotateX) * obj_world;

  glm::mat4 projM = glm::perspective(fovy, (float)WIDTH/HEIGHT, NEAR, FAR);
  glm::vec4 obj_proj = projM * obj_cam;

  return glm::vec4(obj_proj.x, obj_proj.y, obj_proj.z, obj_proj.w);
}

void generateModel (const modelClass model, const modelWorld modelParam, const float scale, const Vec3 color) {
  modelClass modelInst;

  for (const glm::vec4& vertex : model.vertices) {
    glm::vec4 scaled_vertex = glm::vec4(vertex.x*scale, vertex.y*scale, vertex.z*scale, 1.0f);
    glm::vec4 transVertx = transformVertexWorld(scaled_vertex, modelParam);
    modelInst.vertices.push_back(transVertx);
  }
  modelInst.name = model.name;
  modelInst.color = color;
  modelsWorld.push_back(modelInst);
}

void cameraInputs (Cam camera) {
  for (const modelClass& modelWorldspace : modelsWorld) {
    modelClass modelInst1;
    for (const glm::vec4& vertex : modelWorldspace.vertices) {
      glm::vec4 transVertx = transformToScreen(vertex, camera);
      modelInst1.vertices.push_back(transVertx);
    }
    modelInst1.name = modelWorldspace.name;
    modelInst1.color = modelWorldspace.color;
    modelsCam.push_back(modelInst1);
  }
}

void generateWorld (const std::vector<modelClass> models) {
  modelWorld modelParam;

  for (const modelClass& model : models) {
    if (model.name == "wall1") {
      modelParam.position = {2.0f, -2.0f, -4.0f};
      modelParam.angle = glm::radians(45.0f);
      modelParam.axis = {0.0f, 0.0f, 1.0f};
      float scale = 3.0;
      Vec3 color = {150, 130, 50};

      //generateModel(model, modelParam, scale, color);
    }
    if (model.name == "box") {
      modelParam.position = {0.0f, -2.0f, -3.0f};
      modelParam.angle = glm::radians(180.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 0.5;
      Vec3 color = {50, 134, 200};

      generateModel(model, modelParam, scale, color);
    }
    if (model.name == "table") {
      modelParam.position = {0.0f, 0.0f, -4.0f};
      modelParam.angle = glm::radians(180.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 2.0;
      Vec3 color = {190, 134, 170};

      generateModel(model, modelParam, scale, color);
    }
    if (model.name == "teapot") {
      modelParam.position = {0.0f, 0.0f, -4.0f};
      modelParam.angle = glm::radians(180.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 2.0;
      Vec3 color = {200, 124, 70};

      //generateModel(model, modelParam, scale, color);
    }
  }
}