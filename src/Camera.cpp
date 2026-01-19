#include <Camera.h>
#include "RubiksCube.h"

void Camera::SetOrthographic(float near, float far)
{
    m_Near = near;
    m_Far = far;

    // Rest Projection and View matrices
    m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, near, far);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

void Camera::setPerspective(float FOVdegree, float near, float far)
{
    m_Near = near;
    m_Far = far;
    float aspect = (float)m_Width / (float)m_Height;

    m_Projection = glm::perspective(glm::radians(FOVdegree), aspect, near, far);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}



/////////////////////
// Input Callbacks //
/////////////////////

void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        RubiksCube &cube = RubiksCube::getInstance();
        switch (key)
        {
            case GLFW_KEY_A:
                cube.setRotationAngle(180.0f);
                break;
            case GLFW_KEY_Z:
                cube.setRotationAngle(90.0f);
                break;
            case GLFW_KEY_SPACE:
                cube.changeRotationDirection();
                break;
            case GLFW_KEY_R:
                cube.rotateFace(3);
                break;
            case GLFW_KEY_L:
                cube.rotateFace(2);
                break;
            case GLFW_KEY_U:
                cube.rotateFace(4);
                break;
            case GLFW_KEY_D:
                cube.rotateFace(5);
                break;
            case GLFW_KEY_B:
                cube.rotateFace(1);
                break;
            case GLFW_KEY_F:
                cube.rotateFace(0);
                break;
            case GLFW_KEY_UP:
                cube.rotateCube(X_AXIS);
                break;
            case GLFW_KEY_DOWN:
                cube.rotateCube(-X_AXIS);
                break;
            case GLFW_KEY_LEFT:
                cube.rotateCube(-Y_AXIS);
                break;  
            case GLFW_KEY_RIGHT:
                cube.rotateCube(Y_AXIS);
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        std::cout << "MOUSE LEFT Click" << std::endl;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        std::cout << "MOUSE RIGHT Click" << std::endl;
    }
}

void CursorPosCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    camera->m_NewMouseX = camera->m_OldMouseX - currMouseX;
    camera->m_NewMouseY = camera->m_OldMouseY - currMouseY;
    camera->m_OldMouseX = currMouseX;
    camera->m_OldMouseY = currMouseY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        std::cout << "MOUSE LEFT Motion" << std::endl;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        std::cout << "MOUSE RIGHT Motion" << std::endl;
    }
}

void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
        return;
    }

    std::cout << "SCROLL Motion" << std::endl;
}

void Camera::EnableInputs(GLFWwindow* window)
{
    // Set camera as the user pointer for the window
    glfwSetWindowUserPointer(window, this);

    // Handle key inputs
    glfwSetKeyCallback(window, (void(*)(GLFWwindow *, int, int, int, int)) KeyCallback);

    // Handle cursor buttons
    glfwSetMouseButtonCallback(window, (void(*)(GLFWwindow *, int, int, int)) MouseButtonCallback);

    // Handle cursor position and inputs on motion
    glfwSetCursorPosCallback(window , (void(*)(GLFWwindow *, double, double)) CursorPosCallback);

    // Handle scroll inputs
    glfwSetScrollCallback(window, (void(*)(GLFWwindow *, double, double)) ScrollCallback);
}