#pragma once

#include <filesystem>
#include "model.hpp"

namespace fs = std::filesystem;

int loadModels();
extern std::vector<Model> models;