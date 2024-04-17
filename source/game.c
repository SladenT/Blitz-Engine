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
#include <pthread.h>
#include <unistd.h>

GLFWwindow* mainWindow;
Camera*     mainCamera;

// Press Once flags
int selectPO = -1;
int movePO   = -1;

double deltaTime;

struct moveData
{
    uint64_t id;
    vec3     moveTo;
};

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

    cam_TranslateCameraBy(mainCamera, (vec3){0.0f, 3.0, 4.0f});
}

static void TestThread(void* arguments)
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
        cam_TranslateCameraBy(mainCamera, (vec3){0.0f,0.0f,-3.0 * deltaTime});
    }
    if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){0.0f,0.0f,3.0 * deltaTime});
    }
    if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){-3.0 * deltaTime,0.0f,0.0f});
    }
    if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam_TranslateCameraBy(mainCamera, (vec3){3.0 * deltaTime,0.0f,0.0f});
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
        uint64_t ent = p_CheckRaycast(ray, 0x00000004);
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
            float dist = p_CheckRaycastDist(ray, 0x00000002);
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
                pthread_create(&thread, NULL, TestThread, data);
            }
        }
    }
    // Concurrency for Threads;
    cycleCount++;
}