/*******************************************************************************************
*	Collisions and intersections of common types - for now, only Rays and Axis Aligned
*   Bounding Boxes.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "entity.h"
#include "collider.h"
#include "render3d.h"
#include <stdio.h>
#include <math.h>

static bool AABBCollision (Rect3D a, Rect3D b);

bool c_CheckCollisions(uint64_t ID1, PhysicBody* p1, uint64_t ID2, PhysicBody* p2, double deltaTime)
{
    ColliderContainer c1 = p1->col;
    ColliderContainer c2 = p2->col;
    Entity* ent1 = e_GetEntity(ID1);
    Entity* ent2 = e_GetEntity(ID2);
    if ((c1.colliderType == cl_NONE) || (c2.colliderType == cl_NONE))
    {
        printf("Collider does not exist, so collision cannot be tested");
        return false;
    }
    if (((c1.colliderType & cl_AABB) == cl_AABB) && ((c2.colliderType & cl_AABB) == cl_AABB))
    {
        //AABBCollision
        Rect3D* rA = (Rect3D*)c1.mem;
        Rect3D rectA = (Rect3D){rA->x + c1.offset[0] + ent1->position[0] + ((ent1->scale[0]-1)*-1) + p1->velocity[0]*deltaTime, 
                                rA->y + c1.offset[1] + ent1->position[1] + ((ent1->scale[1]-1)*-1) + p1->velocity[1]*deltaTime, 
                                rA->z + c1.offset[2] + ent1->position[2] + ((ent1->scale[2]-1)*-1) + p1->velocity[2]*deltaTime, 
                                rA->w*ent1->scale[0], rA->d*ent1->scale[2], rA->h*ent1->scale[1]};

        Rect3D* rB = (Rect3D*)c2.mem;
        Rect3D rectB = (Rect3D){rB->x + c2.offset[0] + ent2->position[0] + ((ent2->scale[0]-1)*-1) + p2->velocity[0]*deltaTime, 
                                rB->y + c2.offset[1] + ent2->position[1] + ((ent2->scale[1]-1)*-1) + p2->velocity[1]*deltaTime, 
                                rB->z + c2.offset[2] + ent2->position[2] + ((ent2->scale[2]-1)*-1) + p2->velocity[2]*deltaTime, 
                                rB->w*ent2->scale[0], rB->d*ent2->scale[2], rB->h*ent2->scale[1]};
        return AABBCollision(rectA, rectB);
    }
}

void c_SetDefaultAABB(PhysicBody* body)
{
    body->col.colliderType = cl_AABB;
    Rect3D* colbox1 = malloc(sizeof(Rect3D));
    colbox1->x = 0;
    colbox1->y = 0;
    colbox1->z = 0;
    colbox1->w = 2;
    colbox1->h = 2;
    colbox1->d = 2;
    body->col.mem = colbox1;
}

void c_SetDefaultAABBTrigger(PhysicBody* body, void* event)
{
    body->col.colliderType = cl_AABB;
    Rect3D* colbox1 = malloc(sizeof(Rect3D) + sizeof(ColliderTrigger));
    colbox1->x = 0;
    colbox1->y = 0;
    colbox1->z = 0;
    colbox1->w = 2;
    colbox1->h = 2;
    colbox1->d = 2;
    ColliderTrigger* trigger = colbox1 + 1;         // Increments by the size of the entire Rect3D
    trigger->event = event;
    body->col.mem = colbox1;
}

// Returns -1 at no intersection
float c_RayAABBIntersection(Ray r, Rect3D a, uint64_t entID)
{
    Entity* ent = e_GetEntity(entID);
    float t0 = (a.x+ent->position[0] +                       ((ent->scale[0])*-1) - r.origin[0]) * (1/r.dir[0]);
    float t1 = (a.x+ent->position[0] + (a.w*ent->scale[0]) + ((ent->scale[0])*-1) - r.origin[0]) * (1/r.dir[0]);
    if (a.x+ent->position[0] + ((ent->scale[0]-1)*-1) - r.origin[0] == 0 && r.dir[0] == 0)
    {
        t0 = 0;
    }
    float tmin = fmin(t0, t1);
    float tmax = fmax(t0, t1);

    t0 = (a.y+ent->position[1] +                       ((ent->scale[1])*-1) - r.origin[1]) * (1/r.dir[1]);
    t1 = (a.y+ent->position[1] + (a.h*ent->scale[1]) + ((ent->scale[1])*-1) - r.origin[1]) * (1/r.dir[1]);
    if (a.y+ent->position[1] + ((ent->scale[1]-1)*-1) - r.origin[1] == 0 && r.dir[1] == 0)
    {
        t0 = 0;
    }
    tmin = fmax(tmin, fmin(t0, t1));
    tmax = fmin(tmax, fmax(t0, t1));

    t0 = (a.z+ent->position[2] +                       ((ent->scale[2])*-1) - r.origin[2]) * (1/r.dir[2]);
    t1 = (a.z+ent->position[2] + (a.d*ent->scale[2]) + ((ent->scale[2])*-1) - r.origin[2]) * (1/r.dir[2]);
    if (a.z+ent->position[2] + ((ent->scale[2]-1)*-1) - r.origin[2] == 0 && r.dir[2] == 0)
    {
        t0 = 0;
    }
    tmin = fmax(tmin, fmin(t0, t1));
    tmax = fmin(tmax, fmax(t0, t1));

    return tmax > fmax(tmin, 0.0) ? tmin : -1;
}

Ray c_GetMouseRay(Camera c)
{
    Ray ray = {0};
    double mouseX;
    double mouseY;
    glfwGetCursorPos(r3d_getMainWindow(), &mouseX, &mouseY);
    float x = (2.0f*mouseX)/(float)r3d_GetScreenWidth() - 1.0f;
    float y = 1.0f - (2.0f*mouseY)/(float)r3d_GetScreenHeight();
    
    // Get our camera matrices
    mat4 view;
    cam_GetCamTransform(c, view);
    mat4 proj;
    glmc_mat4_copy(c.projection, &proj);

    // Invert them
    mat4 viewInv, projInv;
    glmc_mat4_inv(view, viewInv);
    glmc_mat4_inv(proj, projInv);

    // Get our clipping plane by multiplying the inverse projection
    versor clip = {x, y, -1.0, 1.0};
    versor eye;
    glm_mat4_mulv(projInv, clip, eye);
    eye[2] = -1;
    eye[3] =  0;

    // Then get our direciton by multiplying the inverse view matrix
    versor direc;
    glm_mat4_mulv(viewInv, eye, direc);
    vec3 direction = {direc[0], direc[1], direc[2]};
    glmc_vec3_normalize(direction);

    glmc_vec3_copy(c.position, ray.origin); 
    glmc_vec3_copy(direction, ray.dir);
    return ray;
}


static bool AABBCollision (Rect3D a, Rect3D b)
{
    return (a.x <= (b.x+b.w) && (a.w+a.x) >= b.x) &&
           (a.y <= (b.h+b.y) && (a.h+a.y) >= b.y) &&
           (a.z <= (b.d+b.z) && (a.d+a.z) >= b.z);
}