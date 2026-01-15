#pragma once

#include "RubiksCube.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class CubeController {
    private:
        RubiksCube cube;
        bool isClockWise;
        float rotationAngle;
        glm::mat4 trans;
        glm::mat4 rot;
        glm::mat4 scl;
        CubeController(): 
            cube(RubiksCube()), isClockWise(true), rotationAngle(90.0f),
            trans(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f))),
            rot(glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f))),
            scl(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f))) { rot = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f)); }

    public:
        static CubeController &getInstance() {
            static CubeController instance;
            return instance;
        }

        void changeRotationDirection() { isClockWise = !isClockWise; }

        void enlargeRotationAngle() {
            rotationAngle *= 2.0f;
            if (rotationAngle > 180.0f) rotationAngle = 180.0f;
        }

        void minimizeRotationAngle() {
            rotationAngle *= 0.5f;
            if (rotationAngle < 90.0f) rotationAngle = 90.0f;
        }

        glm::mat4 getModelMatrix() const { return trans * rot * scl; }
        void rotateCube(const glm::vec3 &axis)
        {
            bool cw = isClockWise ? -1.0f : 1.0f;
            rot = glm::rotate(rot, glm::radians(cw * rotationAngle), axis);
        }

        RubiksCube& getCube() { return cube; }
};