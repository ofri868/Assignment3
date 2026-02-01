#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "RubiksCube.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

#include <Debugger.h>
#include <Shader.h>

class Camera
{
    private:
        // View and Projection
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);

        // View matrix paramters
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 8.0f);
        glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Projection matrix parameters
        float m_Near = 10.0f; 
        float m_Far = 100.0f;
        int m_Width;
        int m_Height;

        // Orthographic Projection parameters
        float m_Left = -1.0f;
        float m_Right = 1.0f;
        float m_Bottom = -1.0f; 
        float m_Top = 1.0f;

        // Color picking mode
        bool m_ColorPicking = false;

        // Scene objects for color picking
        VertexArray* m_Vao = nullptr;
        IndexBuffer* m_Ibo = nullptr;
        Shader* m_Shader = nullptr;

        // Picked cubie under mouse cursor
        Cubie* m_PickedCubie = nullptr;
        float m_PickedDepth = 0.0f;

        // Update Viewing matrix
        void updateViewMatrix();

    public:
        // Prevent the camera from jumping around when first clicking left click
        double m_OldMouseX = 0.0;
        double m_OldMouseY = 0.0;
        double m_NewMouseX = 0.0;
        double m_NewMouseY = 0.0;
    public:
        Camera(int width, int height)
            : m_Width(width), m_Height(height) {};

        // Update Projection matrix for Orthographic mode
        void SetOrthographic(float near, float far);

        // Update Projection matrix for Perspective mode
        void setPerspective(float FOVdegree, float near, float far);

        // Handle camera inputs
        void EnableInputs(GLFWwindow* window);

        // Update camera position
        void updatePosition(const float delta);

        // Rotates camera postion according to newMouseX and newMouseY
        void rotate();

        // Traclate camera postion according to newMouseX and newMouseY
        void translate();

        // Update the size of the viewport
        void updateSize(int width, int height);

        // Get whether color picking mode is enabled
        bool isColorPicking() const { return m_ColorPicking; }

        // Toggle color picking mode
        void toggleColorPicking() { m_ColorPicking = !m_ColorPicking; };

        // Set the buffers and shader for color picking
        void setBuffers(VertexArray* va, IndexBuffer* ib, Shader* shader);

        // Pick the cubie under the mouse cursor
        void pickCubie(double x, double y);

        // Rotate cubie under mouse cursor
        void rotateCubie();

        // Translate cubie under mouse cursor
        void translateCubie();

        // Return the axis of the mouse
        glm::vec3 m_XAxis() { return glm::normalize(glm::cross(m_Position - m_Orientation, m_Up)); }
        glm::vec3 m_YAxis() { return glm::normalize(m_Up); }

        inline glm::mat4 GetViewMatrix() const { return m_View; }
        inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }
};