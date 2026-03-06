#pragma once
#include <glm/glm.hpp>

class Camera3D
{

  public:
    Camera3D();
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    float yaw;
    float pitch;

    float sensitivity = 1.0f;
    float move_speed  = 1.0f;

    glm::mat4 getViewMatrix() const;

    void moveCamera(glm::vec2 movement, float deltaTime);
    void mouseMoveCamera(float xoffset, float yoffset);

  private:
    void _updateCameraVectors();
};
