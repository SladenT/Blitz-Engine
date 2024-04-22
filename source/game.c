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
#include "material.h"
#include "utility.h"
#include "gui.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

GLFWwindow* mainWindow;
Camera*     mainCamera;

// Press Once flags
short selectPO = -1;
short movePO   = -1;

double deltaTime;

uint64_t gameState = 0;
uint64_t selectMode = 0;

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

// Graphics data
MeshData tree;
MeshData Character3D;
MeshData bush;
Shader s;

// This is an abomination since rand() only goes up to max of an 16 bit unsigned integer, so we have to do some shifts to fill in the gaps
// However, there are still some minor gaps, in a of one bit, and a gap of two in b.  So technically not very random, but it does the do.
static double GetFloatInRange(double min, double max)
{
	max -= min;
	uint32_t a = rand()>>4, b = rand()>>5;
    a = a << 16;
    b = b << 16;
    a = a + rand();
    b = b + rand();
	double res = ((a*0x1p26)+b) * (1.0/0x1p53);
	max = res * max;
	max += min;
	return max;
}

// Selection
uint64_t selectedEntity = UINT64_MAX;

uint64_t GetGameState(void)
{
    return gameState;
}
void SetGameState(uint64_t state)
{
    gameState = state;
}

static void MouseWheelHandler(GLFWwindow* window, double xoffset, double yoffset)
{
    // yOffset is the mouse scroll moving
    if (gameState == 1)
    {
        cam_BeginLerp(mainCamera->position, (vec3){mainCamera->position[0], mainCamera->position[1]+(copysign(1, yoffset))*-1, mainCamera->position[2]}, 1.0f, deltaTime);
    }
}

static void GenTrees(int count)
{
    for (int i = 0; i < count; i++)
    {
        float xRange = GetFloatInRange(-40, -5);
        if (GetFloatInRange(0,1) > 0.5f)
        {
            xRange = GetFloatInRange(5, 40);
        }
        float yRange = GetFloatInRange(-40, -5);
        if (GetFloatInRange(0,1) > 0.5f)
        {
            yRange = GetFloatInRange(5, 40);
        }
        uint64_t treeent = e_CreateEntity();
        e_SetEnitityScale(treeent, (vec3) {1.5,1.5,1.5});
        e_SetEnitityPosition(treeent, (vec3) {xRange*2,-3.5,yRange*1.892});
        PhysicBody* e4p = p_MakePhysicBody(treeent, true);
        c_SetDefaultAABB(e4p);
        Rect3D* rect2 = e4p->col.mem;
        r3d_GenerateFromMeshData(tree, s, treeent, mat_CreateDefaultMaterial(5, treeent, false));
    }
}

static void GenBushes(int count)
{
    for (int i = 0; i < count; i++)
    {
        float xRange = GetFloatInRange(-40, -1);
        if (GetFloatInRange(0,1) > 0.5f)
        {
            xRange = GetFloatInRange(1, 40);
        }
        float yRange = GetFloatInRange(-40, -1);
        if (GetFloatInRange(0,1) > 0.5f)
        {
            yRange = GetFloatInRange(1, 40);
        }
        /* uint64_t treeent = e_CreateEntity();
        e_SetEnitityScale(treeent, (vec3) {1.5,1.5,1.5}); */
        uint64_t treeent = e_CreateEntity();
        e_SetEnitityScale(treeent, (vec3) {0.02,0.02,0.02});
        e_SetEnitityPosition(treeent, (vec3) {xRange*2,-4.2,yRange*1.892});
        r3d_GenerateFromMeshData(bush, s, treeent, mat_CreateDefaultMaterial(2, treeent, false));
    }
}

static uint64_t GenUnit(int x, int y)
{
    uint64_t ent2 = e_CreateEntity();
    e_SetEnitityPosition(ent2, (vec3) {x,-3.9,y});
    PhysicBody* e1p = p_MakePhysicBody(ent2, false);
    c_SetDefaultAABB(e1p);
    e1p->mask += object_UNITALLY;
    Rect3D* rect = e1p->col.mem;
    rect->y = 1;
    r3d_GenerateFromMeshData(Character3D, s, ent2, mat_CreateDefaultMaterial(1, ent2, false));
    return ent2;
}


void GameInitialize(void)
{
    mainWindow = r3d_getMainWindow();
    mainCamera = cam_GetMainCamera();
    glfwSetScrollCallback(mainWindow, MouseWheelHandler);

    cam_TranslateCameraBy(mainCamera, (vec3){0.0f, 8.0, 8.0f});

    // Initial objects
    MeshData box = res_ImportMesh("../res/meshes/box.obj");
    bush = res_ImportMesh("../res/meshes/bush_01.obj");
    Character3D = res_ImportMesh("../res/meshes/Character3D.obj");
    tree = res_ImportMesh("../res/meshes/lowpolypine.obj");
    MeshData castle = res_ImportMesh("../res/meshes/SimpleCastle.obj");

    s = sh_BuildShader("defdir.vs", "defdir.fs");
    Shader tile = sh_BuildShader("tileable.vs", "tileable.fs");
    /* uint64_t ent = e_CreateEntity();
    //e_SetEnitityScale(ent, (vec3) {0.03,0.03,0.03});
    e_SetEnitityPosition(ent, (vec3) {5.0,0,0});
    PhysicBody* e2p = p_MakePhysicBody(ent, false);
    c_SetDefaultAABB(e2p);
    e2p->mask += object_UNITALLY; */

    /* uint64_t ent2 = e_CreateEntity();
    e_SetEnitityPosition(ent2, (vec3) {-5.0,0,0});
    PhysicBody* e1p = p_MakePhysicBody(ent2, false);
    c_SetDefaultAABB(e1p);
    e1p->mask += object_UNITALLY;
    Rect3D* rect = e1p->col.mem;
    rect->y = 1; */

    

    uint64_t ent4 = e_CreateEntity();
    e_SetEnitityPosition(ent4, (vec3) {0,-5,0});
    e_SetEnitityScale(ent4, (vec3) {200,1,200});
    PhysicBody* e3p = p_MakePhysicBody(ent4, true);
    c_SetDefaultAABB(e3p);
    e3p->mask += object_GROUND;

    uint64_t ent5 = e_CreateEntity();
    e_SetEnitityPosition(ent5, (vec3) {0,-4,0});
    PhysicBody* e4p = p_MakePhysicBody(ent5, true);
    c_SetDefaultAABB(e4p);
    Rect3D* rect2 = e4p->col.mem;
    rect2->z = -3.8;
    rect2->d = 5.8;
    rect2->w = 6.5;
    e4p->mask += object_BUILDING;
    GenTrees(100);

    //r3d_GenerateFromMeshData(box, s, ent, mat_CreateDefaultMaterial(1, ent, false));
    //r3d_GenerateFromMeshData(Character3D, s, ent2, mat_CreateDefaultMaterial(1, ent2, false));
    r3d_GenerateFromMeshData(box, tile, ent4, mat_CreateDefaultMaterial(4, ent4, false));
    r3d_GenerateFromMeshData(castle, s, ent5, mat_CreateDefaultMaterial(6, ent5, false));
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
    PhysicBody* pb = p_GetPBFromEntityID(entID);
    free(data);
    if (moving[entID])
    {
        checkMove[entID] = true;
        while(moving[entID]){}
    }
    float distance = glmc_vec3_distance(from, goTo);
    vec3 pos;
    vec3 rot;
    float newDist = glmc_vec3_distance(from, goTo);
    checkMove[entID] = false;
    vec3 moveVec;
    while (newDist >= 1.0f)
    {
        if (count <= cycleCount)
        {
            moving[entID] = true;
            // Movement
            e_GetEntityPosition(entID, &pos);
            glmc_vec3_sub(pos, goTo, &moveVec);
            glmc_vec3_normalize(&moveVec);
            pb->velocity[0] = -moveVec[0]*5;
            pb->velocity[2] = -moveVec[2]*5;
            newDist = glmc_vec3_distance(pos, goTo);
            count = cycleCount + 1;

            // Rotation
            float xAng = acos(moveVec[0]);
            float yAng = asin(moveVec[2]);
            float angle = glmc_vec3_angle((vec3){-1,0,0}, (vec3){moveVec[0], 0, moveVec[2]});
            if (moveVec[2] < 0)
            {
                angle = (2*3.14) - angle;
                
            }
            e_SetEnitityRotation(entID, (vec3){0, angle + (3.14/2), 0});
            // Non Physics Movement
            /* e_GetEntityPosition(entID, &pos);
            moving[entID] = true;
            vec3 move;
            glmc_vec3_lerp(from, goTo, t, move);
            e_SetEnitityPosition(entID, move);
            count = cycleCount + 1;
            t += (deltaTime/distance) * 10;
            newDist = glmc_vec3_distance(pos, goTo); */
            if (checkMove[entID] || count > 1000000) // Sanity check
            {
                break;
            }
        }
    }
    pb->velocity[0] = 0;
    pb->velocity[2] = 0;
    // Kill thread
    pthread_detach(pthread_self());
    moving[entID] = false;
    pthread_exit(pthread_self());
}

void SpawnUnitFromCastle()
{
    uint64_t entity = GenUnit(2, 2);
    Entity* ent = e_GetEntity(entity);
    pthread_t thread;
    struct moveData *data = malloc(sizeof(struct moveData));
    data->id = entity;
    data->moveTo[0] = 0;
    data->moveTo[1] = -4.1 + (ent->scale[1]);
    data->moveTo[2] = 6;
    pthread_create(&thread, NULL, MoveUnit, data);
}

void PlayerControls(double d_deltaTime)
{
    deltaTime = d_deltaTime;
    cam_LerpLoop(deltaTime);
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
        if (!gui_CheckMouse())
        {
            selectPO = 0;
            Ray ray = c_GetMouseRay(*mainCamera);
            uint64_t hitmask;
            uint64_t ent = p_CheckRaycast(ray, object_UNITALLY + object_BUILDING, &hitmask);
            if (ent != UINT64_MAX && ((hitmask & object_UNITALLY) == object_UNITALLY))
            {
                selectedEntity = ent;
                selectMode = 0;
                gui_ShowMenu(0);
            }
            else if((hitmask & object_BUILDING) == object_BUILDING)
            {
                // Change control mode to building menu
                selectedEntity = ent;
                selectMode = 1;
                gui_ShowMenu(1);
            }
            else
            {
                selectedEntity = UINT64_MAX;
                selectMode = 0;
                gui_ShowMenu(0);
            }
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
        if (selectedEntity != UINT64_MAX && (selectMode == 0))
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