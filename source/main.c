/*******************************************************************************************
*	Entry for execution for our program
*
*   Created by Davis Teigeler
********************************************************************************************/
#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include "render3D.h"

int main(void)
{
    GLFWwindow* window = r3d_InitWindowRender();
    while (!glfwWindowShouldClose(window))
	{
        r3d_RenderPass(window);
    }
    glfwTerminate();
	return 0;
}