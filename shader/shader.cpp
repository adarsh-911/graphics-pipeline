#include <bits/stdc++.h>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../obj_loader/obj_loader.h"
#include "../texturing/texture.h"
#include "shader.h"

std::vector<modelClass> models;
std::vector<modelClass> modelsWorld;
std::vector<modelClass> modelsCam;
std::vector<modelIdx> modelTriangleInd;
std::vector<modelTexClass> modelTexCords;
std::vector<modelIdx> modelTexCordsInd;
std::vector<modelNormalClass> modelNormals;
std::vector<modelIdx> modelNormInd;
std::vector<Texture> modelTexColors;
std::vector<glm::vec3> lightSources;
Cam cameraParam(glm::vec3(0.0f), glm::vec3(0.0f));

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

  std::string pwd = fs::current_path().string();
  std::string models_path = pwd + "/models";

  for (const auto& dir_entry : fs::directory_iterator(models_path)) {
    if (!dir_entry.is_directory())
        continue;

    std::string base_name;
    fs::path obj_path, tex_path;

    for (const auto& file : fs::directory_iterator(dir_entry.path())) {
      if (file.is_regular_file()) {
        fs::path path = file.path();
        std::string ext = path.extension().string();
        std::string name = path.stem().string(); // filename without extension

        if (ext == ".obj")
          obj_path = path;

        if (ext == ".jpg" or ext == ".png")
          tex_path = path;
      }
    }

    if (!obj_path.empty() && !tex_path.empty() &&
      obj_path.stem() == tex_path.stem()) {
      
      OBJLoader loader;
      if (!loader.load(obj_path)) return 1;
      else {
        modelClass m;
        modelIdx mInd;
        modelTexClass mTex;
        modelIdx mTxInd;
        modelNormalClass mNorm;
        modelIdx mNormIdx;

        for (Vec4& vertex : loader.getVertices()) m.vertices.push_back(normalizeModel(vertex));
        for (Vec2uv& texVertex : loader.getTexCords()) mTex.texcoords.push_back(texVertex);
        for (Vec3& normalVertex : loader.getNormals()) mNorm.normals.push_back(normalVertex); 

        mInd.idx = loader.getTriangleInd();
        //mTex.texcoords = loader.getTexCords();
        mTxInd.idx = loader.getTexInd();
        mNormIdx.idx = loader.getNormalInd();
        m.name = obj_path.stem().string();
        m.ind = models.size();

        std::string modelName = obj_path.stem().string();
        mInd.name = modelName;
        mTex.name = modelName;
        mTxInd.name = modelName;
        mNorm.name = modelName;
        mNormIdx.name = modelName;
        
        models.push_back(m);
        modelTriangleInd.push_back(mInd);
        modelTexCords.push_back(mTex);
        modelTexCordsInd.push_back(mTxInd);
        modelNormals.push_back(mNorm);
        modelNormInd.push_back(mNormIdx);
      }
      Texture texInst;
      int w, h;
      std::vector<Color> colors = loadTexture(tex_path.string().c_str(), w, h);
      texInst.pixels = colors;
      texInst.width = w;
      texInst.height = h;
      texInst.name = tex_path.stem().string();
      modelTexColors.push_back(texInst);
    }
  }
  return 0;
}

glm::vec4 transformVertexWorld (const glm::vec4& vertex, const modelWorld modelParam) {

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

void generateModel (const Object type, const modelClass model, const modelWorld modelParam, const float scale, const Vec3 color, const int ind) {
  modelClass modelInst;
  glm::vec3 sum(0.0f);

  for (const glm::vec4& vertex : model.vertices) {
    glm::vec4 scaled_vertex = glm::vec4(vertex.x*scale, vertex.y*scale, vertex.z*scale, 1.0f);
    glm::vec4 transVertx = transformVertexWorld(scaled_vertex, modelParam);
    //if (type.light) sum += glm::vec3(transVertx.x/transVertx.w, transVertx.y/transVertx.w, transVertx.z/transVertx.w);
    if (type.light) sum += glm::vec3(transVertx.x, transVertx.y, transVertx.z);
    modelInst.vertices.push_back(transVertx);
    //std::cout << transVertx.x << transVertx.y << transVertx.z << "\n";
  }
  modelInst.name = model.name;
  modelInst.color = color;
  modelInst.ind = ind;
  modelsWorld.push_back(modelInst);

  if (type.light) lightSources.push_back(sum/static_cast<float>(modelInst.vertices.size()));
  //std::cout << sum.x/static_cast<float>(modelInst.vertices.size()) << sum.y/static_cast<float>(modelInst.vertices.size()) << sum.z/static_cast<float>(modelInst.vertices.size()) << "\n";
  //std::cout << "\n\n";
}

void cameraInputs (Cam camera) {
  cameraParam = camera;
  for (const modelClass& modelWorldspace : modelsWorld) {
    modelClass modelInst1;
    for (const glm::vec4& vertex : modelWorldspace.vertices) {
      glm::vec4 transVertx = transformToScreen(vertex, camera);
      modelInst1.vertices.push_back(transVertx);
    }
    modelInst1.name = modelWorldspace.name;
    modelInst1.color = modelWorldspace.color;
    modelInst1.ind = modelWorldspace.ind;
    modelsCam.push_back(modelInst1);
  }
}

void generateWorld (const std::vector<modelClass> models) {
  modelWorld modelParam;

  for (const modelClass& model : models) {
    Object type;
    if (model.name == "light") {
      type.light = true;
      modelParam.position = {-4.5f, -7.0f, -6.0f};
      modelParam.angle = glm::radians(0.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 0.25;
      Vec3 color = {150, 130, 50};

      generateModel(type, model, modelParam, scale, color, model.ind);
    }
    if (model.name == "wall") {
      type.object == true;
      modelParam.position = {0.0f, 0.0f, -6.0f};
      modelParam.angle = glm::radians(45.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 5.0;
      Vec3 color = {150, 130, 50};

      //generateModel(type, model, modelParam, scale, color, model.ind);
    }
    if (model.name == "box") {
      type.object == true;
      modelParam.position = {0.0f, -2.0f, -3.0f};
      modelParam.angle = glm::radians(0.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 1.0;
      Vec3 color = {50, 134, 200};

      //generateModel(type, model, modelParam, scale, color, model.ind);
    }
    if (model.name == "table") {
      type.object == true;
      modelParam.position = {0.0f, 0.0f, -4.0f};
      modelParam.angle = glm::radians(180.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 2.0;
      Vec3 color = {190, 133, 170};

      generateModel(type, model, modelParam, scale, color, model.ind);
    }
    if (model.name == "teapot") {
      type.object == true;
      modelParam.position = {0.0f, 0.0f, -4.0f};
      modelParam.angle = glm::radians(180.0f);
      modelParam.axis = {0.0f, 0.0f, -1.0f};
      float scale = 2.0;
      Vec3 color = {200, 124, 70};

      //generateModel(type, model, modelParam, scale, color, model.ind);
    }
  }
}
