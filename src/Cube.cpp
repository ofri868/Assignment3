#include "Cube.h"

Cube::Cube(float x, float y, float z) :
    vertices{
        // positions           // colors           // texCoords
        // Front Face          // red
        x-0.5f, y-0.5f, z+0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // Front-bottom-left
        x+0.5f, y-0.5f, z+0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Front-bottom-right
        x+0.5f, y+0.5f, z+0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Front-top-right
        x-0.5f, y+0.5f, z+0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // Front-top-left

        // Back Face           // orange
        x-0.5f, y-0.5f, z-0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 0.0f, // Back-bottom-left
        x+0.5f, y-0.5f, z-0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f, // Back-bottom-right
        x+0.5f, y+0.5f, z-0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 1.0f, // Back-top-right
        x-0.5f, y+0.5f, z-0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f,  // Back-top-left

        // Left Face           // blue
        x-0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Back-bottom-left
        x-0.5f, y-0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Front-bottom-left
        x-0.5f, y+0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Front-top-left
        x-0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // Back-top-left

        // Right Face          // green
        x+0.5f, y-0.5f, z+0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Back-bottom-right
        x+0.5f, y-0.5f, z-0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Front-bottom-right
        x+0.5f, y+0.5f, z-0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // Front-top-right
        x+0.5f, y+0.5f, z+0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Back-top-right

        // Top Face            // white
        x-0.5f, y+0.5f, z+0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Back-top-left
        x+0.5f, y+0.5f, z+0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Back-top-right
        x+0.5f, y+0.5f, z-0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Front-top-right
        x-0.5f, y+0.5f, z-0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // Front-top-left

        // Bottom Face         // yellow
        x-0.5f, y-0.5f, z-0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Back-bottom-left
        x+0.5f, y-0.5f, z-0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Back-bottom-right
        x+0.5f, y-0.5f, z+0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f, // Front-bottom-right
        x-0.5f, y-0.5f, z+0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Front-bottom-left

    },

    indices{
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    }

{}
Cube::Cube(): Cube(0.0f, 0.0f, 0.0f)
{}