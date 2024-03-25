/*******************************************************************************************
*	Collisions and intersections of common types - for now, only Rays and Axis Aligned
*   Bounding Boxes.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "entity.h"
#include <stdio.h>

static bool AABBCollision (Rect3D a, Rect3D b);

bool c_CheckCollisions(uint64_t ID1, ColliderContainer c1, uint64_t ID2, ColliderContainer c2)
{
    Entity* ent1 = e_GetEntity(ID1);
    Entity* ent2 = e_GetEntity(ID2);
    if (((c1.colliderType & cl_NONE) == cl_NONE) || ((c2.colliderType & cl_NONE) == cl_NONE))
    {
        printf("Collider does not exist, so collision cannot be tested");
        return false;
    }
    if (((c1.colliderType & cl_AABB) == cl_AABB) && ((c2.colliderType & cl_AABB) == cl_AABB))
    {
        //AABBCollision
        Rect3D* rA = (Rect3D*)c1.mem;
        Rect3D rectA = (Rect3D){rA->x + c1.offset[0] + ent1->position[0], rA->y + c1.offset[1] + ent1->position[1], 
                                rA->z + c1.offset[2] + ent1->position[2], rA->w, rA->d, rA->h};

        Rect3D* rB = (Rect3D*)c2.mem;
        Rect3D rectB = (Rect3D){rB->x + c2.offset[0] + ent2->position[0], rB->y + c2.offset[1] + ent2->position[1], 
                                rB->z + c2.offset[2] + ent2->position[2], rB->w, rB->d, rB->h};
        return AABBCollision(rectA, rectB);
    }
}


static bool AABBCollision (Rect3D a, Rect3D b)
{
    return (a.x <= b.w && a.w >= b.x) &&
           (a.y <= b.h && a.h >= b.y) &&
           (a.z <= b.d && a.d >= b.z);
}