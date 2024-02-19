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
#include "structs.h"
#include <stdio.h>

static double lastTime;

int main(void)
{
    e_InitEntities();
    GLFWwindow* window = r3d_InitWindowRender();
    res_InitImport();
    MeshData bush = res_ImportMesh("../res/meshes/bush_01.obj");

    Shader s = sh_BuildShader("def.vs", "def.fs");
    uint64_t ent = e_CreateEntity();
    //Model *m = r3d_GenerateModelOne(bush.vertices, bush.vertCount*5, s, ent, mat_CreateDefaultMaterial(2, ent, false));
    Model *m = r3d_GenerateFromMeshData(bush, s, ent, mat_CreateDefaultMaterial(2, ent, false));
    vec3 scale = {0.03,0.03,0.03};
    glm_scale(e_GetEntityTransform(ent), scale);
    m->vertexCount = bush.indexCount;


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