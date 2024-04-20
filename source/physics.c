/*******************************************************************************************
*	Physics systems for the gameworld
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "memarena.h"
#include "physics.h"
#include "math.h"
#include "entity.h"
#include "collider.h"

Arena* physicArena;
static uint64_t physicCounter = 0;
void* physicBody[65536];

void p_PhysicsInit()
{
    physicArena = MakeDataArena(PhysicBody, 1, 8);
}

PhysicBody* p_MakePhysicBody(uint64_t entityID, bool statec)
{
    PhysicBody* pb = ar_AllocOne(physicArena);
    pb->entity = entityID;
    pb->stat = statec;
    pb->mass = 1;
    pb->friction = 0.5;
    pb->bounce = 0.8;
    pb->offset[0] = 0;
    pb->offset[1] = 0;
    pb->offset[2] = 0;
    pb->col.colliderType = cl_NONE;

    pb->velocity[0] = 0;
    pb->velocity[1] = 0;
    pb->velocity[2] = 0;

    pb->accel[0] = 0;
    pb->accel[1] = -9.8;  // Default gravity
    pb->accel[2] = 0;
    pb->mask = 0x1;
    physicCounter++;
    physicBody[entityID] = pb;
    return pb;
}

PhysicBody* p_GetPBFromEntityID(uint64_t entityID)
{
    PhysicBody* pb = (PhysicBody*)physicBody[entityID];
}

void p_DeletePhysicsBodyFromID(uint64_t entityID)
{
    PhysicBody* pb = physicBody[entityID];
    if (pb->col.colliderType != 0)
    {
        free(pb->col.mem);
    }
    ar_Free(physicArena, pb);
    physicBody[entityID] = 0;
}

void p_DeletePhysicsBody(PhysicBody* pb)
{
    if (pb->col.colliderType != 0)
    {
        free(pb->col.mem);
    }
    physicBody[pb->entity] = 0;
    ar_Free(physicArena, pb);
}

// If we only care about the closest
uint64_t p_CheckRaycast(Ray ray, uint64_t mask)
{
    float lowest = 99999999999.0f;
    uint64_t lowestID = UINT64_MAX;
    for (int i = 0; i < physicCounter; i++)
    {
        PhysicBody* body = ar_ArenaIterator(physicArena, &i);
        if ((body->mask & mask) == 0){continue;}
        float rayDistance = c_RayAABBIntersection(ray, *(Rect3D*)body->col.mem, body->entity);
        if (rayDistance != -1)
        {
            if (rayDistance < lowest)
            {
                lowest = rayDistance;
                lowestID = body->entity;
            }
        }
    }
    if (lowest == 99999999999.0f){return UINT64_MAX;}
    return lowestID;
}

// If we only care about the closest distance
float p_CheckRaycastDist(Ray ray, uint64_t mask)
{
    float lowest = 99999999999.0f;
    for (int i = 0; i < physicCounter; i++)
    {
        PhysicBody* body = ar_ArenaIterator(physicArena, &i);
        if ((body->mask & mask) == 0){continue;}
        float rayDistance = c_RayAABBIntersection(ray, *(Rect3D*)body->col.mem, body->entity);
        if (rayDistance != -1)
        {
            if (rayDistance < lowest)
            {
                lowest = rayDistance;
            }
        }
    }
    if (lowest == 99999999999.0f){return -1.0f;}
    return lowest;
}


// Updates all of our physics bodies positions based upon the forces that have been
// Applied to them, such as gravity, checks for collisions, and rebuilds our spatial
// Octree (IF I HAD ONE!)
void p_PhysicsUpdate(double deltaTime)
{
    for (int i = 0; i < physicCounter; i++)
    {
        PhysicBody* body = ar_ArenaIterator(physicArena, &i);
        if (body->stat){continue;}
        
        // Update velocity based on acceleration
        body->velocity[0] += body->accel[0]*deltaTime;
        body->velocity[1] += body->accel[1]*deltaTime;
        body->velocity[2] += body->accel[2]*deltaTime;
        // Update position based on velocity
        Entity* e1 =  e_GetEntity(body->entity);
        // Remember position before move for collisions
        body->prevPos[0] = e1->position[0];
        body->prevPos[1] = e1->position[1];
        body->prevPos[2] = e1->position[2];
        float moveX = 0, moveY = 0, moveZ = 0;
        if (fabs(body->velocity[0]) > 0.5)
        {
            moveX = body->velocity[0]*deltaTime;
        }
        if (fabs(body->velocity[1]) > 0.5)
        {
            moveY = body->velocity[1]*deltaTime;
        }
        if (fabs(body->velocity[2]) > 0.5)
        {
            moveZ = body->velocity[2]*deltaTime;
        }
        e_SetEnitityPosition(body->entity, (vec3){e1->position[0] + (moveX), 
                             e1->position[1] + (moveY), e1->position[2] + (moveZ)});
        
        // check for collisions
        for (int j = 0; j < physicCounter; j++)
        {
            if (j == i){continue;}
            PhysicBody* body2 = ar_ArenaIterator(physicArena, &j);
            CollisionData* cd = c_CheckCollisions(body->entity, body, body2->entity, body2, deltaTime);
            if (cd->collision)
            {
                if (cd->normal[0] != 0)
                {
                    e_SetEnitityPosition(body->entity, (vec3){body->prevPos[0], body->prevPos[1] + (moveY), body->prevPos[2] + (moveZ)});
                }
                else if (cd->normal[1] != 0)
                {
                    e_SetEnitityPosition(body->entity, (vec3){body->prevPos[0] + (moveX), body->prevPos[1], body->prevPos[2] + (moveZ)});
                }
                else
                {
                    e_SetEnitityPosition(body->entity, (vec3){body->prevPos[0] + (moveX), body->prevPos[1] + (moveY), body->prevPos[2]});
                }

                // Newton's Law (TODO: Change to reflection about the normal)
                body->velocity[0] += -body->velocity[0]*body->bounce*body->friction*body2->friction;
                body->velocity[1] += -body->velocity[1]*2*body->bounce;
                body->velocity[2] += -body->velocity[2]*body->bounce*body->friction*body2->friction;
                // Add velocity to the other body
                if (!body2->stat)
                {
                    body2->velocity[0] += body->velocity[0]*body->bounce*body->friction;
                    body2->velocity[1] += body->velocity[1]*2*body->bounce;
                    body2->velocity[2] += body->velocity[2]*body->bounce*body->friction;
                }
            }
        }
    }
}