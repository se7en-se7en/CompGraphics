#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;

    void moveForward(float distance = 1.0f);
    void moveBackward(float distance = 1.0f);
    void strafeLeft(float distance = 1.0f);
    void strafeRight(float distance = 1.0f);
    void moveUp(float distance = 1.0f);
    void moveDown(float distance = 1.0f);

    void rotate(float xoffset, float yoffset, float sensitivity = 0.1f);
    void adjustSpeed(float offset);

private:
    void updateCameraVectors();

private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;
    float m_Speed;
};

