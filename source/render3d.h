#ifndef render3dh
#define render3dh
#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include "structs.h"
#include "components.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// Initializes our program and sets up the OpenGL contex and our window
GLFWwindow* r3d_InitWindowRender(void);

void r3d_RenderPass(GLFWwindow* window, double deltaTime);
Model* r3d_GenerateModelOne(float vertexData[], int vertAttCount, int indexData[], int indexCount, Shader s, uint64_t entID, uint32_t matID);
Model* r3d_GenerateFromMeshData(MeshData data, Shader s, uint64_t entID, uint32_t matID);















#endif