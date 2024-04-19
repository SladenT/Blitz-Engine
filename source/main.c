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
#include "shader.h"
#include "physics.h"
#include "structs.h"
#include "camera.h"
#include "collider.h"
#include "debug.h"
#include "game.h"
#include <stb_image.h>
#include <stdio.h>

static double lastTime;

void test(const char* var)
{
    printf(var);
}

int main(void)
{
    // Engine Initialization Steps
    e_InitEntities();
    GLFWwindow* window = r3d_InitWindowRender();
    res_InitImport();
    gui_Init(window);
    p_PhysicsInit();
    debug_Init();

    // Game initialization
    GameInitialize();

    

    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
	{
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        if (deltaTime > 0.1)
        {
            deltaTime = 0;
        }
        PlayerControls(deltaTime);
        
        // Engine Updates
        p_PhysicsUpdate(deltaTime);
        r3d_RenderPass(window, deltaTime);
        // Render the GUI last, as it disables the depth bit testing
		gui_Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
	return 0;
}