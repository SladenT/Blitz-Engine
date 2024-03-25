#ifndef physicsh
#define physicsh

#include "components.h"

void p_PhysicsInit();
PhysicBody* p_MakePhysicBody(uint64_t entityID, bool statec);
void p_PhysicsUpdate(double deltaTime);












#endif