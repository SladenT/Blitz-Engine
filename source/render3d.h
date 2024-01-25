#ifndef render3dh
#define render3dh
#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// Initializes our program and sets up the OpenGL contex and our window
GLFWwindow* r3d_InitWindowRender(void);

void r3d_RenderPass(GLFWwindow* window, double deltaTime);
















#endif