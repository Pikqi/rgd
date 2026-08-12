#include "glm/ext/matrix_transform.hpp"
#include <algorithm>
#include <camera3d.h>

Camera3D::Camera3D(glm::vec3 position)
    : position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)),
      up(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(0.0f), pitch(0.0f)

{
    world_up = up;
    _updateCameraVectors();
};

glm::mat4 Camera3D::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
};

void Camera3D::moveCamera(glm::vec2 movement, float deltaTime)
{
    if (glm::length(movement) > 0.0f)
        movement = glm::normalize(movement);

    float velocity = move_speed * deltaTime;

    glm::vec3 right_vec = glm::normalize(glm::cross(front, world_up));

    position += front * movement.y * velocity;
    position += right_vec * movement.x * velocity;
    _updateCameraVectors();
}

void Camera3D::mouseMoveCamera(float xoffset, float yoffset)
{

    xoffset *= -sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    pitch = std::clamp(pitch, -90.0f, 90.0f);

    _updateCameraVectors();
}

void Camera3D::_updateCameraVectors()
{

    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front       = glm::normalize(new_front);

    right = glm::normalize(glm::cross(front, world_up));
    up    = glm::normalize(glm::cross(right, front));
}
