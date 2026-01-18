#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

struct Cubie {
    glm::vec3 position;
    glm::mat4 rotationMatrix;
};

class RubiksCube {
    private:
        // is positive for clockwise, negative for counter-clockwise
        float rotationAngle;

        std::array<Cubie, 27> cubes;

        RubiksCube();

        void rotateFace(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, glm::vec3 axis);

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

        void changeRotationDirection() { rotationAngle *= -1.0f; }

        void setRotationAngle(float degrees);

        Cubie* getCubes() { return cubes.data(); }
};