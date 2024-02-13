#version 420 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;

void main()
{
    gl_Position = proj * view * transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
