/*******************************************************************************************
*	Materials for shader uniforms.  This code defines various material types and provides
*   Ways to create more dynamic materials as well.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "material.h"
#include "memarena.h"
#include <string.h>
DynamicArena* matArena;
void*  idLocs[1048576];  //Index is the entity ID, which contains a memory value that points to our material
int matIDCounter = 0;
static bool init = false;


static void InitMaterials()
{
    // Allocate 1GB of Virtual memory, and 4 pages of physical (4*4096 KB)
    matArena = CreateDynamicArena(1, 4);
}

int mat_CreateDefaultMaterial(int texID, int entityID, bool isUnique)
{
    if (!init)
    {
        InitMaterials();
        init = true;
    }
    // Size of default: 8(for ID) + 4(for size) + 4(for uniform count) + 8 (bool for uniqueCheck) + 128(uniform name buffer) + 4 (int texture array id) + 4 (matattribute size) + 8 (uniform type enum) = 168 bytes
    Material* m = ar_AllocDynamic(matArena, 160);
    m->ID = matIDCounter;
    m->size = 152;
    m->count = 1;
    m->isUnique = isUnique;
    m->matAttributes = m+24;
    strncpy(m->matAttributes->uniformName, "texLayer", 128);
    m->matAttributes->type = ut_Integer;
    m->matAttributes->size = 144;
    *(int*)m->matAttributes->data = texID;
    matIDCounter++;
    idLocs[entityID] = m;
    return m->ID;
}