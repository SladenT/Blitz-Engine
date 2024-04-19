#version 420 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

void main()
{
    float scaleX = length(transform[0].xyz);
    float scaleY = length(transform[1].xyz);
    float scaleZ = length(transform[2].xyz);
    vec2 texTile = vec2(aTexCoord.x * scaleX, aTexCoord.y * scaleZ);
    TexCoord = texTile;
    FragPos = vec3(transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    gl_Position = proj * view * vec4(FragPos, 1.0);
}
