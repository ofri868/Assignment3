#include "RubiksCube.h"
#include <iostream>


std::vector<Cube> RubiksCube::getFace(char faceIndex)
{
    switch (faceIndex)
    {
    case 'F':
        return std::vector<Cube> {
            cubes[0], cubes[1], cubes[2],
            cubes[9], cubes[10],cubes[11],
            cubes[18], cubes[19], cubes[20]
        };
        break;
    case 'B':
        return std::vector<Cube> {
            cubes[6], cubes[7], cubes[8],
            cubes[15], cubes[16], cubes[17],
            cubes[24], cubes[25], cubes[26]
        };
        break;
    case 'L':
        return std::vector<Cube> {
            cubes[0], cubes[3], cubes[6],
            cubes[9], cubes[12], cubes[15],
            cubes[18], cubes[21], cubes[24]
        };
        break;
    case 'R':
        return std::vector<Cube> {
            cubes[2], cubes[5], cubes[8],
            cubes[11], cubes[14], cubes[17],
            cubes[20], cubes[23], cubes[26]
        };
        break;
    case 'U':
        return std::vector<Cube> {
            cubes[18], cubes[19], cubes[20],
            cubes[21], cubes[22], cubes[23],
            cubes[24], cubes[25], cubes[26]
        };
        break;
    case 'D':
        return std::vector<Cube> {
            cubes[0] , cubes[1] , cubes[2] ,
            cubes[3] , cubes[4] , cubes[5] ,
            cubes[6] , cubes[7] , cubes[8]
        };
        break;
    
    default:
        std::cerr << "Error: Invalid face index " << faceIndex << std::endl;
        throw std::invalid_argument("Invalid face index");
        break;
    }
}

RubiksCube::RubiksCube()
{
    cubes.reserve(27);
    int index = 0;
    for (float x = -1.0f; x <= 1.0f; x += 1.0f)
    {
        for (float y = -1.0f; y <= 1.0f; y += 1.0f)
        {
            for (float z = -1.0f; z <= 1.0f; z += 1.0f)
            {
                cubes.push_back(Cube(x, y, z));
                index++;
            }
        }
    }
}

const Cube &RubiksCube::getCubeAt(int index) const
{
    return cubes[index];
}

const float *RubiksCube::getVertices() const{
    float* allVertices = new float[27 * (4 * 6 * 8)];
    for (int i = 0; i < 27; i++)
    {
        const float *cubeVertices = cubes[i].getVertices();
        for (int j = 0; j < 4 * 6 * 8; j++)
        {
            allVertices[i * (4 * 6 * 8) + j] = cubeVertices[j];
        }
    }
    return allVertices;
}

const unsigned int *RubiksCube::getIndices() const
{
    unsigned int* allIndices = new unsigned int[27 * (6 * 6)];
    for (int i = 0; i < 27; i++)
    {
        const unsigned int *cubeIndices = cubes[i].getIndices();
        for (int j = 0; j < 6 * 6; j++)
        {
            allIndices[i * (6 * 6) + j] = cubeIndices[j] + i * (4 * 6);
        }
    }
    return allIndices;
}

const size_t RubiksCube::getVerticesSize() const
{
    size_t totalSize = 0;
    for(const Cube &cube : cubes) {
        totalSize += cube.getVerticesSize();
    }
    return totalSize;
}

const size_t RubiksCube::getIndicesSize() const
{
    size_t totalSize = 0;
    for(const Cube &cube : cubes) {
        totalSize += cube.getIndicesSize();
    }
    return totalSize;
}