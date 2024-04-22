#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;

// Gets texture unit from the main function
uniform sampler2D tex0;


void main()
{
    FragColor = vec4(normal, 0.0);
}