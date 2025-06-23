#include "../modelLoader/loadModels.hpp"
#include "../vertexTransform/transform.hpp"
#include "lightning.hpp"

float SHINE = 32.0f;
float AMBIENT_STRENGTH = 0.1f;
float DIFFUSE_STRENGTH = 0.3f;
float SPECULAR_STRENGTH = 1.0f;
glm::vec3 LIGHTCOLOR = glm::vec3(1.0f, 1.0f, 1.0f);

float constant = 1.0;
float linear = 0.05;
float quadratic = 0.005;

float RANGE = 18.0f;

glm::vec3 reflect (const glm::vec3& I, const glm::vec3& N) {
    return I - 2.0f * glm::dot(I, N) * N;
}

glm::vec3 lightIntensity (int modelIdx, int triIdx, glm::vec3& barycentric, glm::mat4& INV) {
  glm::vec3 total(0.0f);

  if (lightSources.size() != 0) {
    
    Model* model = models.data();
    idx idx1 = model[modelIdx].getIndices(1)[triIdx];

    glm::vec4 ver0(cameraSpace[modelIdx].vertices[idx1.a]);
    glm::vec4 ver1(cameraSpace[modelIdx].vertices[idx1.b]);
    glm::vec4 ver2(cameraSpace[modelIdx].vertices[idx1.c]);

    float x_proj = (ver0.x/ver0.w)*barycentric.x + (ver1.x/ver1.w)*barycentric.y + (ver2.x/ver2.w)*barycentric.z;
    float y_proj = (ver0.y/ver0.w)*barycentric.x + (ver1.y/ver1.w)*barycentric.y + (ver2.y/ver2.w)*barycentric.z;
    float z_proj = 1 / ((ver0.w/ver0.z)*barycentric.x + (ver1.w/ver1.z)*barycentric.y + (ver2.w/ver2.z)*barycentric.z);
    float w_proj = 1 / ((1/ver0.w)*barycentric.x + (1/ver1.w)*barycentric.y + (1/ver2.w)*barycentric.z);

    glm::vec4 world_vertex = INV * glm::vec4(x_proj*w_proj, y_proj*w_proj, z_proj*w_proj, w_proj);
    float x = world_vertex.x;
    float y = world_vertex.y;
    float z = world_vertex.z;

    glm::vec3 N(0.0f);
    
    if (model[modelIdx].getNormals().size() != 0) {
      idx idx2 = model[modelIdx].getIndices(3)[triIdx];
      glm::vec3 normal = model[modelIdx].getNormals()[idx2.a]*barycentric.x + model[modelIdx].getNormals()[idx2.b]*barycentric.y + model[modelIdx].getNormals()[idx2.c]*barycentric.z;
      
      N = glm::normalize(glm::vec3(normal.x, normal.y, normal.z));
    }
    else N = glm::normalize(glm::cross(glm::vec3(ver1-ver0), glm::vec3(ver2-ver1)));

    for (glm::vec3& lightSrc : lightSources) {
      glm::vec3 L = glm::normalize(glm::vec3(lightSrc.x, lightSrc.y, lightSrc.z) - glm::vec3(x, y, z));
      glm::vec3 V = glm::normalize(cameraInst.position - glm::vec3(x, y, z));
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
      float dFactor = std::clamp(1.0 - distance / RANGE, 0.0, 1.0);

      total = ambient + diffuse + specular + glm::vec3(dFactor, dFactor, dFactor);
    }
  }
  else total = LIGHTCOLOR * AMBIENT_STRENGTH;
  return total;
}
