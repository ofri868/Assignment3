#include "Cube.h"
#include <vector>
#pragma once

class RubiksCube {
    // RubiksCube class definition
    private:
        std::vector<Cube> cubes; // 3x3x3 Rubik's Cube made of 27 smaller cubes
        std::vector<Cube> getFace(char faceIndex); // Method to get a specific face of the Rubik's Cube
    public:
        RubiksCube();
        const Cube& getCubeAt(int index) const;
        const float *getVertices() const;
        const unsigned int *getIndices() const;
        const size_t getVerticesSize() const;
        const size_t getIndicesSize() const;
};