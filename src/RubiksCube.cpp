#include "RubiksCube.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

#define sign(x) ((x) < 0 ? -1 : 1)

const float OFFSET = CUBIE_SCALE;
const glm::vec3 X_AXIS = CUBE_X_AXIS;
const glm::vec3 Y_AXIS = CUBE_Y_AXIS;
const glm::vec3 Z_AXIS = CUBE_Z_AXIS;

RubiksCube::RubiksCube(): rotationAngle(glm::radians(-90.0f)), cubes{}
{
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            for(int z = -1; z <= 1; z++) {
                int index = (x + 1) * 9 + (y + 1) * 3 + (z + 1);
                cubes[index].position = OFFSET * glm::vec3(x, y, z);
                cubes[index].rotationMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f));
            }
        }
    }
}

glm::vec3 getOriginOfRotation(int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
{
    glm::vec3 origin = glm::vec3(
        (minX + maxX) / 2.0f,
        (minY + maxY) / 2.0f,
        (minZ + maxZ) / 2.0f
    );
    return origin;
}

void RubiksCube::rotate(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, glm::vec3 axis)
{
    // Pre-computing affine map for repositiong the cubes
    glm::vec3 originOfRotation = OFFSET * getOriginOfRotation(minX, minY, minZ, maxX, maxY, maxZ);
    glm::mat4 fromOrigin = glm::translate(glm::mat4(1.0f), -originOfRotation);
    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), originOfRotation);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotationAngle, axis);
    glm::mat4 transformation = toOrigin * rotation * fromOrigin;

    // Pre-computing affine map for repositiong indices of cubes array
    glm::vec3 originOfRotationIndex = getOriginOfRotation(minX, minY, minZ, maxX, maxY, maxZ);
    glm::mat4 fromOriginIndex = glm::translate(glm::mat4(1.0f), -originOfRotationIndex);
    glm::mat4 toOriginIndex = glm::translate(glm::mat4(1.0f), originOfRotationIndex);
    glm::mat4 indexTranformation = toOriginIndex * rotation * fromOriginIndex;

    // saving new states of the cubes
    std::array<Cubie, 27> newCubes = cubes;

    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                int index = (x + 1) * 9 + (y + 1) * 3 + (z + 1);
                // compute new orientation and position
                cubes[index].rotationMatrix = rotation * cubes[index].rotationMatrix;
                cubes[index].position = glm::vec3(transformation * glm::vec4(cubes[index].position, 1.0f));
                // compute new index after rotation
                glm::vec3 newIndexVector = glm::vec3(indexTranformation * glm::vec4(x , y , z , 1.0f));
                int newX = glm::round(newIndexVector.x);
                int newY = glm::round(newIndexVector.y);
                int newZ = glm::round(newIndexVector.z);
                int newIndex = (newX + 1) * 9 + (newY + 1) * 3 + (newZ + 1);
                newCubes[newIndex] = cubes[index];
            }
        }
    }

    cubes = newCubes;
}

void RubiksCube::rotateFace(int faceIndex)
{
    switch(faceIndex) {
        case 0: // Front face
            // x from -1 to 1, y from -1 to 1, z = 1
            rotate(-1, -1, 1, 1, 1, 1, Z_AXIS);
            break;
        case 1: // Back face
            // x from -1 to 1, y from -1 to 1, z = -1
            rotate(-1, -1, -1, 1, 1, -1, -Z_AXIS);
            break;
        case 2: // Left face
            // x = -1, y from -1 to 1, z from -1 to 1
            rotate(-1, -1, -1, -1, 1, 1, -X_AXIS);
            break;
        case 3: // Right face
            // x = 1, y from -1 to 1, z from -1 to 1
            rotate(1, -1, -1, 1, 1, 1, X_AXIS);
            break;
        case 4: // Top face
            // y = 1, x from -1 to 1, z from -1 to 1
            rotate(-1, 1, -1, 1, 1, 1, Y_AXIS);
            break;
        case 5: // Bottom face
            // y = -1, x from -1 to 1, z from -1 to 1
            rotate(-1, -1, -1, 1, -1, 1, -Y_AXIS);
            break;
        default:
            break;
    }
}

void RubiksCube::rotateCube(glm::vec3 axis) { rotate(-1, -1, -1, 1, 1, 1, axis); }

void RubiksCube::setRotationAngle(float degrees)
{
    if(degrees > 180) {
        degrees = 180;
    } else if(degrees < 90) {
        degrees = 90;
    }

    rotationAngle = sign(rotationAngle) * glm::radians(degrees);
}