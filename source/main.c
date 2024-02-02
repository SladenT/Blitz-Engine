/*******************************************************************************************
*	Entry for execution for our program
*
*   Created by Davis Teigeler
********************************************************************************************/
#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include "render3D.h"
#include "material.h"
#include "entity.h"
#include "resimport.h"
#include <stdio.h>

static double lastTime;

int main(void)
{
    e_InitEntities();
    GLFWwindow* window = r3d_InitWindowRender();
    res_InitImport();
    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
	{
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        r3d_RenderPass(window, deltaTime);
    }
    glfwTerminate();
	return 0;
}