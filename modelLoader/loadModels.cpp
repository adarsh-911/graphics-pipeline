#include "loadModels.hpp"

std::vector<Model> models;

int loadModels() {
  std::string pwd = fs::current_path().string();
  std::string models_path = pwd + "/all_models";

  for (const auto& dir_entry : fs::directory_iterator(models_path)) {
    if (!dir_entry.is_directory())
        continue;

    fs::path obj_path, tex_path;

    for (const auto& file : fs::directory_iterator(dir_entry.path())) {
      if (file.is_regular_file()) {
        fs::path path = file.path();
        std::string ext = path.extension().string();

        if (ext == ".obj") obj_path = path;
        if (ext == ".jpg" or ext == ".png") tex_path = path;
      }
    }
    if (!obj_path.empty() && !tex_path.empty() &&
      obj_path.stem() == tex_path.stem()) {

      Model modelInst;
      if (!modelInst.load(obj_path)) return 1;
      else {
        std::string name = obj_path.stem().string();
        modelInst.name = name;
        modelInst.loadTexture(tex_path.string().c_str(), modelInst.texWidth, modelInst.texHeight);

        if (name == "light") {
          modelInst.light = true;
          modelInst.position = {0.0f, -5.0f, -4.0f};
          modelInst.angle = glm::radians(90.0f);
          modelInst.axis = {1.0f, 0.0f, 0.0f};
          modelInst.scaleFactor = 0.25;
          modelInst.color = {255, 255, 255};

          models.push_back(modelInst);
        }
        if (name == "wall") {
          modelInst.light = false;
          modelInst.position = {0.0f, 0.0f, -15.0f};
          modelInst.angle = glm::radians(0.0f);
          modelInst.axis = {1.0f, 0.0f, 0.0f};
          modelInst.scaleFactor = 8.0;
          modelInst.color = {150, 130, 50};

          models.push_back(modelInst);
        }
        if (name == "wall1") {
          modelInst.light = false;
          modelInst.position = {-8.0f, 0.0f, -7.0f};
          modelInst.angle = glm::radians(90.0f);
          modelInst.axis = {0.0f, 1.0f, 0.0f};
          modelInst.scaleFactor = 8.0;
          modelInst.color = {150, 130, 50};

          models.push_back(modelInst);
        }
        if (name == "wall2") {
          modelInst.light = false;
          modelInst.position = {8.0f, 0.0f, -7.0f};
          modelInst.angle = glm::radians(90.0f);
          modelInst.axis = {0.0f, -1.0f, 0.0f};
          modelInst.scaleFactor = 8.0;
          modelInst.color = {150, 130, 50};

          models.push_back(modelInst);
        }
        if (name == "floor") {
          modelInst.light = false;
          modelInst.position = {0.0f, 8.0f, -7.0f};
          modelInst.angle = glm::radians(90.0f);
          modelInst.axis = {1.0f, 0.0f, 0.0f};
          modelInst.scaleFactor = 8.0;
          modelInst.color = {150, 130, 50};

          models.push_back(modelInst);
        }
        if (name == "roof") {
          modelInst.light = false;
          modelInst.position = {0.0f, -8.0f, -7.0f};
          modelInst.angle = glm::radians(90.0f);
          modelInst.axis = {-1.0f, 0.0f, 0.0f};
          modelInst.scaleFactor = 8.0;
          modelInst.color = {150, 130, 50};

          models.push_back(modelInst);
        }
        if (name == "card") {
          modelInst.light = false;
          modelInst.position = {0.0f, 0.0f, -14.93f};
          modelInst.angle = glm::radians(0.0f);
          modelInst.axis = {0.0f, 1.0f, 0.0f};
          modelInst.scaleFactor = 7.0;
          modelInst.color = {150, 130, 50};

          models.push_back(modelInst);
        }
        if (name == "box") {
          modelInst.light = false;
          modelInst.position = {0.0f, -2.0f, -3.0f};
          modelInst.angle = glm::radians(30.0f);
          modelInst.axis = {1.0f, 0.0f, 0.0f};
          modelInst.scaleFactor = 1.5;
          modelInst.color = {50, 134, 200};

          //models.push_back(modelInst);
        }
        if (name == "table") {
          modelInst.light = false;
          modelInst.position = {0.0f, 7.0f, -3.0f};
          modelInst.angle = glm::radians(90.0f);
          modelInst.axis = {1.0f, 0.0f, 0.0f};
          modelInst.scaleFactor = 6.5;
          modelInst.color = {190, 133, 170};

          models.push_back(modelInst);
        }
        if (name == "chair") {
          modelInst.light = false;
          modelInst.position = {0.0f, 6.0f, -2.0f};
          modelInst.angle = glm::radians(180.0f);
          modelInst.axis = {0.0f, 0.0f, 1.0f};
          modelInst.scaleFactor = 1.5;
          modelInst.color = {190, 133, 170};

          //models.push_back(modelInst);
        }
        if (name == "pyramid") {
          modelInst.light = false;
          modelInst.position = {0.0f, 3.0f, -0.5f};
          modelInst.angle = glm::radians(180.0f);
          modelInst.axis = {0.0f, 0.0f, 1.0f};
          modelInst.scaleFactor = 1.0;
          modelInst.color = {200, 124, 70};

          //models.push_back(modelInst);
        }
      }
    }
  }
  return 0;
}
