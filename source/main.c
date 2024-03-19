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
#include "camera.h"
#include <stb_image.h>
#include <stdio.h>

static double lastTime;

int main(void)
{
    e_InitEntities();
    GLFWwindow* window = r3d_InitWindowRender();
    res_InitImport();
    gui_Init(window);

    Camera *c = cam_GetMainCamera();
    glmc_translate(c->transform, (vec3){0.0f,-3.0,-4.0f});

    MeshData box = res_ImportMesh("../res/meshes/box.obj");
    MeshData bush = res_ImportMesh("../res/meshes/bush_01.obj");

    Shader s = sh_BuildShader("def.vs", "def.fs");
    uint64_t ent = e_CreateEntity();
    //e_SetEnitityScale(ent, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent, (vec3) {5.0,0,0});

    uint64_t ent2 = e_CreateEntity();
    //e_SetEnitityScale(ent2, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent2, (vec3) {-5.0,0,0});

    uint64_t ent3 = e_CreateEntity();
    e_SetEnitityScale(ent3, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent3, (vec3) {0.0,0,-3.0});


    Model *m = r3d_GenerateFromMeshData(box, s, ent, mat_CreateDefaultMaterial(1, ent, false));
    Model *m2 = r3d_GenerateFromMeshData(box, s, ent2, mat_CreateDefaultMaterial(1, ent2, false));
    Model *m3 = r3d_GenerateFromMeshData(bush, s, ent3, mat_CreateDefaultMaterial(2, ent3, false));

    m->vertexCount  = box.indexCount;
    m2->vertexCount  = box.indexCount;
    m3->vertexCount  = bush.indexCount;

    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
	{
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        r3d_RenderPass(window, deltaTime);
		gui_Render();
        glfwPollEvents();
    }
    glfwTerminate();
	return 0;
}