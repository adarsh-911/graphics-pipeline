#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"
#include "../clip/clip.h"
#include "../texturing/texture.h"

float SHINE = 8.0f;
float AMBIENT_STRENGTH = 0.06f;
float DIFFUSE_STRENGTH = 0.8f;
float SPECULAR_STRENGTH = 1.0f;
glm::vec3 LIGHTCOLOR = glm::vec3(1.0f, 1.0f, 1.0f);

float RANGE  = 18.0f;

float constant = 1.0;
float linear = 0.05;
float quadratic = 0.005;

const int WIDTH = 600;
const int HEIGHT = 600;

glm::vec3 reflect (const glm::vec3& I, const glm::vec3& N) {
    return I - 2.0f * glm::dot(I, N) * N;
}

glm::vec3 lightIntensity (int modelInd, int triInd, Vec3 barycentric) {

  modelClass model = modelsCam[modelInd];
  Ind idx = modelTriangleInd[modelInd].idx[triInd];
  
  glm::vec3 v0(model.vertices[idx.v0].x, model.vertices[idx.v0].y, model.vertices[idx.v0].z);
  glm::vec3 v1(model.vertices[idx.v1].x, model.vertices[idx.v1].y, model.vertices[idx.v1].z);
  glm::vec3 v2(model.vertices[idx.v2].x, model.vertices[idx.v2].y, model.vertices[idx.v2].z);

  float x_proj = (model.vertices[idx.v0].x/model.vertices[idx.v0].w)*barycentric.x + (model.vertices[idx.v1].x/model.vertices[idx.v1].w)*barycentric.y + (model.vertices[idx.v2].x/model.vertices[idx.v2].w)*barycentric.z;
  float y_proj = (model.vertices[idx.v0].y/model.vertices[idx.v0].w)*barycentric.x + (model.vertices[idx.v1].y/model.vertices[idx.v1].w)*barycentric.y + (model.vertices[idx.v2].y/model.vertices[idx.v2].w)*barycentric.z;
  float z_proj = 1 / ((model.vertices[idx.v0].w/model.vertices[idx.v0].z)*barycentric.x + (model.vertices[idx.v1].w/model.vertices[idx.v1].z)*barycentric.y + (model.vertices[idx.v2].w/model.vertices[idx.v2].z)*barycentric.z);
  float w_proj = 1 / ((1/model.vertices[idx.v0].w)*barycentric.x + (1/model.vertices[idx.v1].w)*barycentric.y + (1/model.vertices[idx.v2].w)*barycentric.z);

  glm::vec4 inter_vertex = glm::inverse(transform_matrix) * glm::vec4(x_proj*w_proj, y_proj*w_proj, z_proj*w_proj, w_proj);
  float x = inter_vertex.x;
  float y = inter_vertex.y;
  float z = inter_vertex.z;
  
  glm::vec3 N(0.0f);

  if ((modelNormals[modelInd]).normals.size() != 0) {

    modelNormalClass modelNorms = modelNormals[modelInd];
    Ind idx1 = modelNormInd[modelInd].idx[triInd];
    Vec3 normal = modelNorms.normals[idx1.v0]*barycentric.x + modelNorms.normals[idx1.v1]*barycentric.y + modelNorms.normals[idx1.v2]*barycentric.z;
    
    N = glm::normalize(glm::vec3(normal.x, normal.y, normal.z));
  }
  else N = glm::normalize(glm::cross(v1-v0, v2-v1));
  
  glm::vec3 total(0.0f);
  if (lightSources.size() != 0) {
    
    for (glm::vec3& lightSrc : lightSources) {
      glm::vec3 L = glm::normalize(glm::vec3(lightSrc.x, lightSrc.y, lightSrc.z) - glm::vec3(x, y, z));
      glm::vec3 V = glm::normalize(cameraParam.pos - glm::vec3(x, y, z));
      glm::vec3 R = reflect(-L, -N);

      float diff = std::max(glm::dot(-N, L), 0.0f);
      float spec = std::pow(std::max(glm::dot(R, V), 0.0f), SHINE);

      glm::vec3 ambient = LIGHTCOLOR * AMBIENT_STRENGTH;
      glm::vec3 diffuse = diff * LIGHTCOLOR * DIFFUSE_STRENGTH;
      glm::vec3 specular = spec * LIGHTCOLOR * SPECULAR_STRENGTH;

      float distance = glm::length(glm::vec3(lightSrc.x, lightSrc.y, lightSrc.z) - glm::vec3(x, y, z));
      float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);
      float dFactor = std::clamp(1.0 - std::fabs(distance) / RANGE, 0.0, 1.0);

      diffuse *= attenuation;
      specular *= attenuation;
      total += ambient + diffuse + specular + glm::vec3(dFactor, dFactor, dFactor);
      //total = glm::vec3(dFactor, dFactor, dFactor) + ambient;
    }
  }
  else total = LIGHTCOLOR * AMBIENT_STRENGTH;
  return total;
}
