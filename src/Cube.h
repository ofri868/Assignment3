#pragma once

#include <cstddef>

class Cube {
    private:
        float vertices[4 * 6 * 8]; // 4 vertices per face, 6 faces, 8 attributes (position(3), color(3), texCoords(2))
        unsigned int indices[6 * 6]; // 6 indices per face, 6 faces

    public:
        Cube();
        const float *getVertices() const { return vertices; }
        const unsigned int *getIndices() const { return indices; }
        const size_t getVerticesSize() const { return sizeof(vertices); }
        const size_t getIndicesSize() const { return sizeof(indices); }
};