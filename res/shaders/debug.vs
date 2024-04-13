#version 420 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0

uniform mat4 view;
uniform mat4 proj;


void main()
{
    gl_Position = proj * view * vec4(aPos, 1.0);
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}