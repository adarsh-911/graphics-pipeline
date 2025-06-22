#include "shader/shader.h"
#include "clip/clip.h"
#include "clip/zClip.h"
#include "raster/raster.h"
#include <string>

void run_localTest(char* argv[]) {
  glm::vec3 pos = {std::stof(argv[1]), std::stof(argv[2]), std::stof(argv[3])};
  glm::vec3 dir = {std::stof(argv[4]), std::stof(argv[5]), std::stof(argv[6])};
 
  loadModels();
  generateWorld(models);
  cameraInputs(Camera(pos, glm::normalize(dir)));
  clip();
  zClip(1);
  VertexBuffer_update();
  render();
  
  std::string pwd = std::string(std::filesystem::current_path());
  savePPM(pwd + "/output.ppm");
}