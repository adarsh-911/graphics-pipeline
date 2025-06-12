#include "../obj_loader/obj_loader.h"
#include "../shader/shader.h"

float SHINE = 32.0f;
float AMBIENT_STRENGTH = 0.1f;
float DIFFUSE_STRENGTH = 0.8f;
float SPECULAR_STRENGTH = 0.5f;
glm::vec3 LIGHTCOLOR = glm::vec3(1.0f, 1.0f, 1.0f);

glm::vec3 reflect (const glm::vec3& I, const glm::vec3& N) {
    return I - 2.0f * glm::dot(I, N) * N;
}

glm::vec3 lightAmp (int modelInd, int triInd, Vec3 barycentric) {

  modelClass model = modelsWorld[modelInd];
  modelIdx triInds = modelTriangleInd[modelInd];
  Ind idx = triInds.idx[triInd];
  
  //float x = (model.vertices[idx.v0].x/model.vertices[idx.v0].w)*barycentric.x + (model.vertices[idx.v1].x/model.vertices[idx.v1].w)*barycentric.y + (model.vertices[idx.v2].x/model.vertices[idx.v2].w)*barycentric.z;
  //float y = (model.vertices[idx.v0].y/model.vertices[idx.v0].w)*barycentric.x + (model.vertices[idx.v1].y/model.vertices[idx.v1].w)*barycentric.y + (model.vertices[idx.v2].y/model.vertices[idx.v2].w)*barycentric.z;
  
  float x = (model.vertices[idx.v0].x)*barycentric.x + (model.vertices[idx.v1].x)*barycentric.y + (model.vertices[idx.v2].x)*barycentric.z;
  float y = (model.vertices[idx.v0].y)*barycentric.x + (model.vertices[idx.v1].y)*barycentric.y + (model.vertices[idx.v2].y)*barycentric.z;
  float z = (model.vertices[idx.v0].z)*barycentric.x + (model.vertices[idx.v1].z)*barycentric.y + (model.vertices[idx.v2].z)*barycentric.z;

  modelNormalClass modelNorms = modelNormals[modelInd];
  modelIdx normInds = modelNormInd[modelInd];
  Ind idx1 = normInds.idx[triInd];
  Vec3 normal = modelNorms.normals[idx1.v0]*barycentric.x + modelNorms.normals[idx1.v1]*barycentric.y + modelNorms.normals[idx1.v2]*barycentric.z;
  
  glm::vec3 N = glm::normalize(glm::vec3(normal.x, normal.y, normal.z));

  //std::cout << x << y << z << "\n";
  
  glm::vec3 total(0.0f);
  for (glm::vec3& lightSrc : lightSources) {
    glm::vec3 L = glm::normalize(lightSrc - glm::vec3(x, y, z));
    glm::vec3 V = glm::normalize(cameraParam.pos - glm::vec3(x, y, z));
    glm::vec3 R = reflect(-L, -N);

    float diff = std::max(glm::dot(-N, L), 0.0f);
    float spec = std::pow(std::max(glm::dot(R, V), 0.0f), SHINE);

    glm::vec3 ambient = LIGHTCOLOR * AMBIENT_STRENGTH;
    glm::vec3 diffuse = diff * LIGHTCOLOR * DIFFUSE_STRENGTH;
    glm::vec3 specular = spec * LIGHTCOLOR * SPECULAR_STRENGTH;

    total += ambient + diffuse + specular;
    //std::cout << total.x << " " << total.y << " " << total.z << "\n";
  }
  return total;
}
