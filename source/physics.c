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
    physicCounter++;
    return pb;
}

// Does nothing atm, just a reminder that the collision, should it have a type, needs to be manually free'd
// As it is not part of the arena.
void p_DeletePhysicsBody()
{

}

/* typedef struct PhysicBody
{
	uint64_t 		  entity;
	bool 	 		  stat;				// A static physic body is not updated by physics calculations, and is not moved by other physics forces (such as collisions), but
										// still interacts with other physics bodys e.g. a static landmass
	ColliderContainer col;
	float 	 		  mass;
	float			  friction;
	float 			  bounce;
	vec3 	 		  offset;			// This dictates the center of mass, away from entity origin - it does not change the position of the collider.
	vec3 	 		  velocity;
	vec3 	 		  accel;			// Typically, this should only be modified for constant acceleration, such as gravity.  More dynamic forms (such as air resistance)
										// should be custom coded To modify velocity instead
} PhysicBody; */


// Updates all of our physics bodies positions based upon the forces that have been
// Applied to them, such as gravity, checks for collisions, and rebuilds our spatial
// Octree
void p_PhysicsUpdate(double deltaTime)
{
    for (int i = 0; i < physicCounter; i++)
    {
        PhysicBody* body = ar_ArenaIterator(physicArena, &i);
        if (body->stat){continue;}
        // check for collisions
        for (int j = 0; j < physicCounter; j++)
        {
            if (j == i){continue;}
            PhysicBody* body2 = ar_ArenaIterator(physicArena, &j);
            if (c_CheckCollisions(body->entity, body, body2->entity, body2, deltaTime))
            {
                body->velocity[0] += -body->velocity[0]*2*body->bounce;
                body->velocity[1] += -body->velocity[1]*2*body->bounce;
                body->velocity[2] += -body->velocity[2]*2*body->bounce;
                // Add velocity to the other body
            }
        }
        // Update velocity based on acceleration
        body->velocity[0] += body->accel[0]*deltaTime;
        body->velocity[1] += body->accel[1]*deltaTime;
        body->velocity[2] += body->accel[2]*deltaTime;
        // Update position based on velocity
        Entity* e1 =  e_GetEntity(body->entity);
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
    }
}