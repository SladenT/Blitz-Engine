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
    
    vec3 scale = {0.03,0.03,0.03};

    Shader s = sh_BuildShader("def.vs", "def.fs");
    uint64_t ent = e_CreateEntity();
    Model *m = r3d_GenerateFromMeshData(bush, s, ent, mat_CreateDefaultMaterial(2, ent, false));
    glm_scale(e_GetEntityTransform(ent), scale);
    m->vertexCount = bush.indexCount;

    uint64_t ent2 = e_CreateEntity();
    Model *m2 = r3d_GenerateFromMeshData(bush, s, ent2, mat_CreateDefaultMaterial(2, ent2, false));
    m2->vertexCount = bush.indexCount;

    uint64_t ent3 = e_CreateEntity();
    Model *m3 = r3d_GenerateFromMeshData(bush, s, ent3, mat_CreateDefaultMaterial(2, ent3, false));
    m3->vertexCount = bush.indexCount;


    glmc_translate(e_GetEntityTransform(ent2), (vec3){7.0f,0.0f,-5.0f});
    glm_scale(e_GetEntityTransform(ent2), scale);

    glmc_translate(e_GetEntityTransform(ent3), (vec3){-3.0f,0.0f,-5.0f});
    glm_scale(e_GetEntityTransform(ent3), scale);


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