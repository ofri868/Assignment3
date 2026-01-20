#pragma once

#include <glm/glm.hpp>
#include <array>

static constexpr float CUBIE_SCALE = 1.0f;
static constexpr glm::vec3 CUBE_X_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr glm::vec3 CUBE_Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
static constexpr glm::vec3 CUBE_Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);

struct Cubie {
    glm::vec3 position;
    glm::mat4 rotationMatrix;
};

class RubiksCube {
    private:
        // is negative for clockwise, positive for counter-clockwise
        float rotationAngle;

        std::array<Cubie, 27> cubes;

        RubiksCube();

        void rotate(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, glm::vec3 axis);

    public:
        static RubiksCube &getInstance() {
            static RubiksCube instance;
            return instance;
        }
        /*
        FaceIndex: 
            0: Front face,
            1: Back face,
            2: Left face,
            3: Right face,
            4: Top face,
            5: Bottom face
        */
        void rotateFace(int faceIndex);

        void rotateCube(glm::vec3 axis);

        void changeRotationDirection() { rotationAngle *= -1.0f; }

        void setRotationAngle(float degrees);

        Cubie* getCubes() { return cubes.data(); }
};