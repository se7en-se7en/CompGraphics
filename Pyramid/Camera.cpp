#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_Position(position),
    m_WorldUp(up),
    m_Yaw(yaw),
    m_Pitch(pitch),
    m_Speed(1.0f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::moveForward(float distance)
{
    m_Position += m_Front * distance * m_Speed;
}

void Camera::moveBackward(float distance)
{
    m_Position -= m_Front * distance * m_Speed;
}

void Camera::strafeLeft(float distance)
{
    m_Position -= m_Right * distance * m_Speed;
}

void Camera::strafeRight(float distance)
{
    m_Position += m_Right * distance * m_Speed;
}

void Camera::moveUp(float distance)
{
    m_Position += m_Up * distance * m_Speed;
}

void Camera::moveDown(float distance)
{
    m_Position -= m_Up * distance * m_Speed;
}

void Camera::rotate(float xoffset, float yoffset, float sensitivity)
{
    m_Yaw += xoffset * sensitivity;
    m_Pitch += yoffset * sensitivity;

    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    else if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::adjustSpeed(float offset)
{
    m_Speed += offset * m_Speed;
    if (m_Speed < 0.01f)
        m_Speed = 0.01f;
    else if (m_Speed > 10.0f)
        m_Speed = 10.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
