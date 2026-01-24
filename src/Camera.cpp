#include <Camera.h>

#include "Debugger.h"
#include <GLFW/glfw3.h>

const float EPS = 0.5f; 
const float SENSITIVITY = 0.01f;
const glm::vec3 PICKED_COLOR = glm::vec3(0.678431f, 0.917647f, 0.917647f); // default to a color that doesn't exist on the cube: Cyan

void Camera::SetOrthographic(float near, float far)
{
    m_Near = near;
    m_Far = far;

    // Reset Projection and View matrices
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

void Camera::updateViewMatrix()
{
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

void Camera::updatePosition(const float delta)
{
    m_Position += m_Orientation * (delta + EPS);
    updateViewMatrix();
}

void Camera::rotate()
{
    float angleX = m_NewMouseX / glm::pi<float>();
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX * SENSITIVITY, CUBE_Y_AXIS);
    // glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX * SENSITIVITY, m_MouseYAxis());

    float angleY = m_NewMouseY / glm::pi<float>();
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY * SENSITIVITY, CUBE_X_AXIS);
    // glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY * SENSITIVITY, m_MouseXAxis());

    glm::mat4 rot = rotY * rotX;
    m_Position = glm::vec3(rot * glm::vec4(m_Position, 1));
    m_Orientation = glm::vec3(rot * glm::vec4(m_Orientation, 1));

    updateViewMatrix();
}

void Camera::translate()
{
    float stepX = -m_NewMouseX * SENSITIVITY;
    glm::mat4 transX = glm::translate(glm::mat4(1.0f), -1.0f * glm::vec3(stepX, 0.0f, 0.0f));

    float stepY = -m_NewMouseY * SENSITIVITY;
    glm::mat4 transY = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, stepY, 0.0f));

    glm::mat4 trans = transX * transY;

    m_Position = glm::vec3(trans * glm::vec4(m_Position, 1));
    updateViewMatrix();
}

void Camera::updateSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    float aspect = (float)m_Width / (float)m_Height;

    m_Projection = glm::perspective(glm::radians(45.0f), aspect, m_Near, m_Far);
    updateViewMatrix();
}

void Camera::setBuffers(VertexArray* va, IndexBuffer* ib, Shader* shader)
{
    m_Vao = va;
    m_Ibo = ib;
    m_Shader = shader;
}

glm::vec4 encodeColorId(int colorId) {
    int toEncode = colorId + 1;
    float r = (toEncode & 0x000000FF) >>  0;
    float g = (toEncode & 0x0000FF00) >>  8;
    float b = (toEncode & 0x00FF0000) >> 16;

    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1);
}

int decodeColor(const unsigned char color[4]) {
    return (color[0] | color[1] << 8 | color[2] << 16) - 1;
}

void Camera::pickCubie(double x, double y)
{
    // Only pick cubie if in color picking mode and all buffers and shader are set
    if(!m_ColorPicking) { return; }

    if(!m_Vao || !m_Ibo || !m_Shader) {
        std::cout << "Warning: Color picking buffers or shader not set! pickCubie is skipped" << std::endl;
        return;
    }

    // Bind the buffers and shader
    m_Vao->Bind();
    m_Ibo->Bind();
    m_Shader->Bind();

    // Enable color picking mode in the shader
    m_Shader->SetUniform1i("u_picking", 1);

    /* Set white background color */
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    /* Clear the color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get Rubik's Cube instance and its cubes
    RubiksCube& cube = RubiksCube::getInstance();
    Cubie *cubes = cube.getCubes();

    // Draw each cubie with unique color ID
    for(int i = 0; i < 27; i++) {

        /* Initialize the model Translate, Rotate and Scale matrices */
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), cubes[i].position);
        glm::mat4 rot = cubes[i].rotationMatrix;
        glm::mat4 scl = glm::scale(glm::mat4(1.0f), glm::vec3(CUBIE_SCALE));

        /* Calculate Model-View-Projection matrix */
        glm::mat4 model = trans * rot * scl;
        glm::mat4 view = m_View;
        glm::mat4 proj = m_Projection;
        glm::mat4 mvp = proj * view * model;

        // Generate unique color ID based on cubie's index
        glm::vec4 colorId = encodeColorId(i);
        // printf("Cubie i: %d, Color ID RGBA: (%.3f, %.3f, %.3f, %.3f)\n", i, colorId.r, colorId.g, colorId.b, colorId.a);
        // Draw the cubie
        m_Shader->SetUniform4f("u_Color", colorId);
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, m_Ibo->GetCount(), GL_UNSIGNED_INT, nullptr));   
    }

    // Read pixel color under mouse cursor
    unsigned char color[4] = {0};
    glReadPixels((int)x, m_Height - (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    // glReadPixels(centerX, centerY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    printf("Picked Color RGBA: (%x, %x, %x, %x)\n", color[0], color[1], color[2], color[3]);

    // Disable color picking mode in the shader
    m_Shader->SetUniform1i("u_picking", 0);

    // set the picked cubie based on the color ID
    int pickedIndex = decodeColor(color);
    printf("Picked Cubie Index: %d\n", pickedIndex);
    if(pickedIndex < 0 || pickedIndex >= 27) {
        m_PickedCubie = nullptr;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }
    m_PickedCubie = &cubes[pickedIndex];

    // Read depth value under mouse cursor
    m_PickedDepth = 0.0f;
    glReadPixels((int)x, m_Height - (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &m_PickedDepth);

    // Reset the buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Camera::rotateCubie()
{
    if (!m_PickedCubie) return;

    float angleX = (float)-m_NewMouseX / glm::pi<float>();
    float angleY = (float)m_NewMouseY / glm::pi<float>();

    const float sensitivity = 2.0f * SENSITIVITY;
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX * sensitivity, m_YAxis());    
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY * sensitivity, m_XAxis());

    m_PickedCubie->rotationMatrix = rotY * rotX  * m_PickedCubie->rotationMatrix;   
}

void Camera::translateCubie()
{
    if(!m_PickedCubie) return;

    glm::vec4 viewport = glm::vec4(0, 0, m_Width, m_Height);

    // Current 3D position under mouse
    glm::vec3 currentScreenPos = glm::vec3((float)m_OldMouseX, (float)m_Height - (float)m_OldMouseY, m_PickedDepth);
    glm::vec3 currentWorldPos = glm::unProject(currentScreenPos, m_View, m_Projection, viewport);

    // Previous 3D position (Current minus Delta)
    glm::vec3 prevScreenPos = glm::vec3((float)(m_OldMouseX - (-m_NewMouseX)), (float)m_Height - (float)(m_OldMouseY - (-m_NewMouseY)), m_PickedDepth);
    glm::vec3 prevWorldPos = glm::unProject(prevScreenPos, m_View, m_Projection, viewport);

    glm::vec3 worldDelta = currentWorldPos - prevWorldPos;

    m_PickedCubie->position += worldDelta;
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
            case GLFW_KEY_P:
                camera->toggleColorPicking();
                break;
            case GLFW_KEY_UP:
                cube.rotateCube(CUBE_X_AXIS);
                break;
            case GLFW_KEY_DOWN:
                cube.rotateCube(-CUBE_X_AXIS);
                break;
            case GLFW_KEY_LEFT:
                cube.rotateCube(-CUBE_Y_AXIS);
                break;  
            case GLFW_KEY_RIGHT:
                cube.rotateCube(CUBE_Y_AXIS);
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! MouseButtonCallback is skipped" << std::endl;
        return;
    }
    double x, y;
    if (camera->isColorPicking() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        std::cout << "MOUSE LEFT Click" << std::endl;
        glfwGetCursorPos(window, &x, &y);
        camera->pickCubie(x, y);

    }
    else if (camera->isColorPicking() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        std::cout << "MOUSE RIGHT Click" << std::endl;
        glfwGetCursorPos(window, &x, &y);
        camera->pickCubie(x, y);
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

    if (!camera->isColorPicking() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        camera->rotate();
    }
    else if (!camera->isColorPicking() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera->translate();

    } else if(camera->isColorPicking() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {

        camera->translateCubie();

    } else if(camera->isColorPicking() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

        camera->rotateCubie();

    }
}

void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
        return;
    }

    // Zoom in and out
    camera->updatePosition((float)scrollOffsetY);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! Framebuffer Size Callback is skipped" << std::endl;
        return;
    }

    glViewport(0, 0, width, height);

    camera->updateSize(width, height);
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

    // Handle window resize
    glfwSetFramebufferSizeCallback(window, (void(*)(GLFWwindow *, int, int)) FramebufferSizeCallback);
}