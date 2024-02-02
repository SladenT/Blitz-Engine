#ifndef entityh
#define entityh
#include "components.h"

void e_InitEntities();
uint64_t e_CreateEntity();
mat4* e_GetEntityTransform(uint64_t id);



















#endif