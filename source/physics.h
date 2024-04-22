#ifndef physicsh
#define physicsh

#include "components.h"
#include "structs.h"

void p_PhysicsInit();
PhysicBody* p_MakePhysicBody(uint64_t entityID, bool statec);
void p_PhysicsUpdate(double deltaTime);
uint64_t p_CheckRaycast(Ray ray, uint64_t mask, uint64_t *hitmask);
float p_CheckRaycastDist(Ray ray, uint64_t mask);

PhysicBody* p_GetPBFromEntityID(uint64_t entityID);











#endif