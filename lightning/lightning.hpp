#pragma once

#include "../modelLoader/model.hpp"

glm::vec3 lightIntensity (int modelIdx, int triIdx, glm::vec3& barycentric, glm::mat4& INV);