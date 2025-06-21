#include "../objLoader/obj_loader.h"
#include "../shader/shader.h"

float SHINE = 32.0f;
float AMBIENT_STRENGTH = 0.1f;
float DIFFUSE_STRENGTH = 0.3f;
float SPECULAR_STRENGTH = 1.0f;
glm::vec3 LIGHTCOLOR = glm::vec3(1.0f, 1.0f, 1.0f);

float constant = 1.0;
float linear = 0.05;
float quadratic = 0.005;

glm::vec3 reflect (const glm::vec3& I, const glm::vec3& N) {
    return I - 2.0f * glm::dot(I, N) * N;
}

glm::vec3 lightIntensity (int modelInd, int triInd, Vec3 barycentric) {

  modelClass model = modelsWorld[modelInd];
  Ind idx = modelTriangleInd[modelInd].idx[triInd];
  
  glm::vec3 v0(model.vertices[idx.v0].x, model.vertices[idx.v0].y, model.vertices[idx.v0].z);
  glm::vec3 v1(model.vertices[idx.v1].x, model.vertices[idx.v1].y, model.vertices[idx.v1].z);
  glm::vec3 v2(model.vertices[idx.v2].x, model.vertices[idx.v2].y, model.vertices[idx.v2].z);

  float x = (model.vertices[idx.v0].x)*barycentric.x + (model.vertices[idx.v1].x)*barycentric.y + (model.vertices[idx.v2].x)*barycentric.z;
  float y = (model.vertices[idx.v0].y)*barycentric.x + (model.vertices[idx.v1].y)*barycentric.y + (model.vertices[idx.v2].y)*barycentric.z;
  float z = (model.vertices[idx.v0].z)*barycentric.x + (model.vertices[idx.v1].z)*barycentric.y + (model.vertices[idx.v2].z)*barycentric.z;

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

      diffuse *= attenuation;
      specular *= attenuation;
      total += ambient + diffuse + specular;
      float dFactor = std::clamp(1.0 - distance / 5.0, 0.0, 1.0);
      total += glm::vec3(dFactor, dFactor, dFactor);
    }
  }
  else total = LIGHTCOLOR * AMBIENT_STRENGTH;
  return total;
}
