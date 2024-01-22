/*******************************************************************************************
*	Entry for execution for our program
*
*   Created by Davis Teigeler
********************************************************************************************/

#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

int main(void)
{
    if (!glfwInit())
	{
		//We busted
		return 0;
	}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShepRTS", NULL, NULL);
	if (!window)
	{
		// Window or OpenGL context creation failed
		printf("Window broke");
	}
	glfwMakeContextCurrent(window);
    //Setup the function bindings for later versions of openGL using GLAD
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);


    while (!glfwWindowShouldClose(window))
	{
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
		glfwPollEvents();
    }


    glfwTerminate();
	return 0;
}