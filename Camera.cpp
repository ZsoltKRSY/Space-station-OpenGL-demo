#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->worldUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, worldUpDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //return camera position
    glm::vec3 Camera::getCameraPosition() {
        return cameraPosition;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed, bool flyMode) {
        //printf("%f %f %f\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += speed * cameraFrontDirection;
                break;
            case MOVE_BACKWARD:
                cameraPosition -= speed * cameraFrontDirection;
                break;
            case MOVE_RIGHT:
                cameraPosition += speed * cameraRightDirection;
                break;
            case MOVE_LEFT:
                cameraPosition -= speed * cameraRightDirection;
                break;
            case MOVE_UP:
                cameraPosition += speed * cameraUpDirection;
                break;
            case MOVE_DOWN:
                cameraPosition -= speed * cameraUpDirection;
                break;
            default:
                break;
        }

        if (!flyMode) {
            cameraPosition.y = walkY;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        float pitchRadians = glm::radians(pitch),
            yawRadians = glm::radians(yaw);

        glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), pitchRadians, cameraRightDirection);
        glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), yawRadians, cameraUpDirection);
        glm::mat4 cameraRotation = yawRotation * pitchRotation;

        cameraFrontDirection = glm::normalize(glm::vec3(cameraRotation * glm::vec4(cameraFrontDirection, 1.0f)));
        cameraFrontDirection = glm::normalize(cameraFrontDirection);
        
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, worldUpDirection));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}
