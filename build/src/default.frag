#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

// Textures
uniform sampler2D tex0; // Existing texture
uniform sampler2D causticsMap; // Caustics texture

// Light properties
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    // Ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Sample the caustics texture to modify the diffuse component
    float causticsIntensity = texture(causticsMap, texCoord).r; // Assuming red channel stores intensity
    vec3 causticsEffect = causticsIntensity * lightColor * 0.5; // Modulate light color by caustics intensity

    // Combine all lighting effects with caustics effect
    vec3 result = ambient + (diffuse + causticsEffect) + specular;
    FragColor = vec4(result, 1.0); // Ensure alpha is set to 1 for full opacity
}
