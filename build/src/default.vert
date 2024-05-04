#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Normals
layout (location = 1) in vec3 aNormal;
// Colors
layout (location = 2) in vec3 aColor;
// Texture Coordinates
layout (location = 3) in vec2 aTex;


out vec3 color;
out vec2 texCoord;
out vec3 normal;

uniform mat4 camMatrix;
uniform mat4 model;


void main()
{
    // Outputs the positions/coordinates of all vertices
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
    color = aColor;
    texCoord = aTex;
    normal = aNormal;
}