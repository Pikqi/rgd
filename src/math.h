#pragma once
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <glm/glm.hpp>

inline float directionToRotation(glm::vec2 dir)
{
    return glm::degrees(glm::atan(dir.y, dir.x)) + 90.0f;
}
