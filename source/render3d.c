/*******************************************************************************************
 *	Rendering methods and pipeline.  Meshes are logically ordered into groups, and similar
 *  Meshes should be placed in similar groups.
 *
 *   Created by Davis Teigeler
 ********************************************************************************************/
// #include <assimp/cimport.h>

#include "render3d.h"
#include "components.h"
#include "structs.h"
#include "shader.h"
#include "camera.h"
#include <cglm/call.h>
#include <stdio.h>

unsigned int SCR_WIDTH = 960;
unsigned int SCR_HEIGHT = 540;

Shader s;
Model* m;
Camera* c;

double lastTime = 0.0f;

// Test box
float vertices[] = {
	
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // A
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // B
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // A
	
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // F
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // H
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // H
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // D
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // H

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // G
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // F
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // B
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // B
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // G

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // B
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // F
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // F
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // G
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // H
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f  // G
};

/* float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};   */

// #region Callback Definitions
static void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
// #endregion

GLFWwindow* r3d_InitWindowRender(void)
{
    if (!glfwInit())
    {
        // We busted
        return 0;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShepRTS", NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        printf("Window broke");
    }
    glfwMakeContextCurrent(window);
    // Setup the function bindings for later versions of openGL using GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // VSYNC
    glfwSwapInterval(1);
    // Back Face Culling (Counter Clockwise Order)
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    // Enable Z-Buffer
    glEnable(GL_DEPTH_TEST);
    // Setup callback
    glfwSetErrorCallback(error_callback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Temp for test display
    s = sh_BuildShader("def.vs", "def.fs");
    m = malloc(sizeof(Model));
    r3d_GenerateMeshOne(m, vertices);
    c = cam_GetMainCamera();
    mat4 transform;
	versor q;
	glmc_mat4_identity(m->transform);
	glmc_mat4_quat(m->transform, q);
    lastTime = glfwGetTime();
    return window;
}

void r3d_GenerateMeshOne(Model *mod, float vertexData[])
{
    /* uint32_t VAO;  //Vertex Array Object, which stores our VBO and EBO
	uint32_t VBO;  //Vertex Buffer Object, which stores our vertex/texture/misc. data for the shader to use
	uint32_t EBO; */
    glGenVertexArrays(1, &mod->mesh.VAO);
    glGenBuffers(1, &mod->mesh.VBO);
    glBindVertexArray(mod->mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mod->mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, 150*sizeof(float), vertexData, GL_STATIC_DRAW);

    //Verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //UV Coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void r3d_RenderPass(GLFWwindow* window)
{
    double deltaTime = glfwGetTime() - lastTime;
    lastTime = glfwGetTime();
    // TEMPORARY
    int vertexColorLocation = glGetUniformLocation(s.ID, "ourColor");
	int vertexTransformLoc = glGetUniformLocation(s.ID, "transform");
	int vertexViewLoc = glGetUniformLocation(s.ID, "view");
	int vertexProjectionLoc = glGetUniformLocation(s.ID, "proj");

    float gValue = 0.5f;
    float rValue = 1.0f;
    float bValue = 0.5f;

    // TEMPORARY
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

    sh_UseShader(s.ID);
        glUniformMatrix4fv(vertexProjectionLoc, 1, GL_FALSE, (float *)c->projection);
        glUniform4f(vertexColorLocation, rValue, gValue, bValue, 1.0f);
        glUniformMatrix4fv(vertexTransformLoc, 1, GL_FALSE, (float *)m->transform);
        glUniformMatrix4fv(vertexViewLoc, 1, GL_FALSE, (float *)c->transform);

        glBindVertexArray(m->mesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    glfwSwapBuffers(window);
    glfwPollEvents();
}
