#include "modelLoader/loadModels.hpp"
#include "vertexTransform/transform.hpp"
#include "clipping/clip.hpp"
#include "clipping/zClip.hpp"
#include "vertexTransform/transformScreen.hpp"
#include "raster/raster.hpp"
#include "screen.hpp"

void run_localTest(char* argv[]) {

  glm::vec3 pos = {std::stof(argv[1]), std::stof(argv[2]), std::stof(argv[3])};
  glm::vec3 dir = {std::stof(argv[4]), std::stof(argv[5]), std::stof(argv[6])};

  Camera camera;
  camera.position = pos;
  camera.direction = glm::normalize(dir);
  
  //std::cout << "Loading Models...\n";
  //loadModels();
  std::cout << "---------------------------------------------------------------\n";
  std::cout << "Reading models...\n";
  readModels();
  std::cout << "Generating World...\n";
  generateWorld();
  std::cout << "Transforming to camera space...\n";
  cameraInputs(camera);
  std::cout << "Z clipping...\n";
  zClip(false);
  std::cout << "Transforming to screen space...\n";
  transformToScreen();
  std::cout << "X-Y Clipping...\n";
  clip();
  std::cout << "Rasterizing and fragment shading...\n";
  render();
  std::cout << "Done!\n";
  std::cout << "---------------------------------------------------------------\n";
  
  std::string pwd = std::string(std::filesystem::current_path());
  savePPM(pwd + "/output.ppm");
}