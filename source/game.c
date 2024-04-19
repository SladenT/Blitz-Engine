/*******************************************************************************************
*	Various game functions and logic that control the simulation.  NOT a part of the engine
*
*   Created by Davis Teigeler
********************************************************************************************/

#include "render3d.h"
#include "camera.h"
#include "collider.h"
#include "physics.h"
#include "entity.h"
#include "resimport.h"
#include <pthread.h>
#include <unistd.h>

GLFWwindow* mainWindow;
Camera*     mainCamera;

// Press Once flags
short selectPO = -1;
short movePO   = -1;

double deltaTime;

struct moveData
{
    uint64_t id;
    vec3     moveTo;
};

enum EntityType
{
	object_DEFAULT 		= 0x00000001,
	object_GROUND		= 0X00000002,
    object_UNITALLY		= 0X00000004,
    object_UNITENEMY	= 0X00000008,
    object_BUILDING     = 0x00000010
} EntityType;

// Threading Concurrency
uint64_t cycleCount = 0;

// Move Check
bool moving[65536];
bool checkMove[65536];


// Selection
uint64_t selectedEntity = UINT64_MAX;

void GameInitialize(void)
{
    mainWindow = r3d_getMainWindow();
    mainCamera = cam_GetMainCamera();

    cam_TranslateCameraBy(mainCamera, (vec3){0.0f, 8.0, 8.0f});

    // Initial objects
    MeshData box = res_ImportMesh("../res/meshes/box.obj");
    MeshData bush = res_ImportMesh("../res/meshes/bush_01.obj");

    Shader s = sh_BuildShader("defdir.vs", "defdir.fs");
    Shader tile = sh_BuildShader("tileable.vs", "tileable.fs");
    uint64_t ent = e_CreateEntity();
    //e_SetEnitityScale(ent, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent, (vec3) {5.0,0,0});
    PhysicBody* e2p = p_MakePhysicBody(ent, false);
    c_SetDefaultAABB(e2p);
    e2p->mask += object_UNITALLY;

    uint64_t ent2 = e_CreateEntity();
    //e_SetEnitityScale(ent2, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent2, (vec3) {-5.0,0,0});
    PhysicBody* e1p = p_MakePhysicBody(ent2, false);
    c_SetDefaultAABB(e1p);
    e1p->mask += object_UNITALLY;

    uint64_t ent3 = e_CreateEntity();
    e_SetEnitityScale(ent3, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent3, (vec3) {0.0,-3.5,-3.0});

    uint64_t ent4 = e_CreateEntity();
    e_SetEnitityPosition(ent4, (vec3) {0,-5,0});
    e_SetEnitityScale(ent4, (vec3) {40,1,40});
    PhysicBody* e3p = p_MakePhysicBody(ent4, true);
    c_SetDefaultAABB(e3p);
    e3p->mask += object_GROUND;

    r3d_GenerateFromMeshData(box, s, ent, mat_CreateDefaultMaterial(1, ent, false));
    r3d_GenerateFromMeshData(box, s, ent2, mat_CreateDefaultMaterial(1, ent2, false));
    r3d_GenerateFromMeshData(bush, s, ent3, mat_CreateDefaultMaterial(2, ent3, false));
    r3d_GenerateFromMeshData(box, tile, ent4, mat_CreateDefaultMaterial(4, ent4, false));
}

static void MoveUnit(void* arguments)
{
    struct moveData *data = arguments;
    //vec4 data = {(float*)arguments[0], (float*)arguments[0], (float*)arguments[0], (float*)arguments[0]};
    uint64_t entID = data->id;
    vec3 from;
    e_GetEntityPosition(entID, &from);
    vec3 goTo = {data->moveTo[0], data->moveTo[1], data->moveTo[2]};
    float t = 0;
    int count = cycleCount;
    free(data);
    if (moving[entID])
    {
        checkMove[entID] = true;
        while(moving[entID]){}
    }
    float distance = glmc_vec3_distance(from, goTo);
    checkMove[entID] = false;
    while (t <= 1.0f)
    {
        if (count <= cycleCount)
        {
            moving[entID] = true;
            vec3 move;
            glmc_vec3_lerp(from, goTo, t, move);
            e_SetEnitityPosition(entID, move);
            count = cycleCount + 1;
            t += (deltaTime/distance) * 10;
            if (checkMove[entID])
            {
                break;
            }
        }
    }
    // Kill thread
    pthread_detach(pthread_self());
    moving[entID] = false;
}

void PlayerControls(double d_deltaTime)
{
    deltaTime = d_deltaTime;
    if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){0.0f,0.0f,-6.0 * deltaTime});
    }
    if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){0.0f,0.0f,6.0 * deltaTime});
    }
    if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){-6.0 * deltaTime,0.0f,0.0f});
    }
    if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){6.0 * deltaTime,0.0f,0.0f});
    }

    // Selection Controls
    if (glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        if (selectPO == -1){selectPO = 1;}
    }
    if (glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
    {
        selectPO = -1;
    }
    if (selectPO == 1)
    {
        selectPO = 0;
        Ray ray = c_GetMouseRay(*mainCamera);
        uint64_t ent = p_CheckRaycast(ray, object_UNITALLY);
        if (ent != UINT64_MAX)
        {
            selectedEntity = ent;
        }
        else
        {
            selectedEntity = UINT64_MAX;
        }
    }

    // Move Unit Controls
    if (glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        if (movePO == -1){movePO = 1;}
    }
    if (glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
    {
        movePO = -1;
    }
    if (movePO == 1)
    {
        movePO = 0;
        if (selectedEntity != UINT64_MAX)
        {
            Ray ray = c_GetMouseRay(*mainCamera);
            float dist = p_CheckRaycastDist(ray, object_GROUND);
            if (dist != -1)
            {
                Entity* ent = e_GetEntity(selectedEntity);
                float dirX = ray.dir[0] * dist;
                float dirY = ray.dir[1] * dist;
                float dirZ = ray.dir[2] * dist;
                pthread_t thread;
                struct moveData *data = malloc(sizeof(struct moveData));
                data->id = selectedEntity;
                data->moveTo[0] = ray.origin[0] + dirX;
                data->moveTo[1] = (ray.origin[1] + dirY) + (ent->scale[1]);
                data->moveTo[2] = ray.origin[2] + dirZ;
                pthread_create(&thread, NULL, MoveUnit, data);
            }
        }
    }
    // Concurrency for Threads;
    cycleCount++;
}