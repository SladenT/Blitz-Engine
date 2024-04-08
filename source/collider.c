/*******************************************************************************************
*	Collisions and intersections of common types - for now, only Rays and Axis Aligned
*   Bounding Boxes.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "entity.h"
#include <stdio.h>

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


static bool AABBCollision (Rect3D a, Rect3D b)
{
    return (a.x <= (b.x+b.w) && (a.w+a.x) >= b.x) &&
           (a.y <= (b.h+b.y) && (a.h+a.y) >= b.y) &&
           (a.z <= (b.d+b.z) && (a.d+a.z) >= b.z);
}