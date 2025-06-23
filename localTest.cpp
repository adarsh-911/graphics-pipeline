#include "modelLoader/loadModels.hpp"
#include "vertexTransform/transform.hpp"
#include "clipping/clip.hpp"
#include "raster/raster.hpp"
#include "screen.hpp"

void run_localTest(char* argv[]) {

  glm::vec3 pos = {std::stof(argv[1]), std::stof(argv[2]), std::stof(argv[3])};
  glm::vec3 dir = {std::stof(argv[4]), std::stof(argv[5]), std::stof(argv[6])};

  Camera camera;
  camera.position = pos;
  camera.direction = glm::normalize(dir);
  
  loadModels();
  generateWorld();
  cameraInputs(camera);
  clip();
  render();
  
  std::string pwd = std::string(std::filesystem::current_path());
  savePPM(pwd + "/output.ppm");
}