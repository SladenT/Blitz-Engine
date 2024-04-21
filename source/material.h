#ifndef materialh
#define materialh
#include "components.h"
#include "structs.h"

int mat_CreateDefaultMaterial(int texID, int entityID, bool isUnique);
Material* mat_GetMatFromID(int ID);
void mat_SetShaderFromMaterial(Material *mat, Shader s);
MatAtt* mat_GetMaterialAttributeFromID(int ID, const char* name);





















#endif