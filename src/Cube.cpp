#include "Cube.h"

Cube::Cube():
    vertices{
        // positions           // colors           // texCoords
        // Front Face          // red
        -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // Front-bottom-left
         0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Front-bottom-right
         0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Front-top-right
        -0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // Front-top-left

        // Back Face           // orange
        -0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 0.0f, // Back-bottom-left
         0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f, // Back-bottom-right
         0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 1.0f, // Back-top-right
        -0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f,  // Back-top-left

        // Left Face           // blue
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Back-bottom-left
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Front-bottom-left
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Front-top-left
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // Back-top-left

        // Right Face          // green
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Back-bottom-right
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Front-bottom-right
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // Front-top-right
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Back-top-right

        // Top Face            // white
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Back-top-left
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Back-top-right
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Front-top-right
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // Front-top-left

        // Bottom Face         // yellow
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Back-bottom-left
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Back-bottom-right
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f, // Front-bottom-right
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Front-bottom-left

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