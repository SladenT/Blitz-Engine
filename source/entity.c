/*******************************************************************************************
*	Handler for our entities.  Mostly keeps track of IDs for us.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "entity.h"
#include "memarena.h"
#include "components.h"

Arena* entityArena;
int64_t idArr[1048576]; // 8MB of memory, max 1 million entities, for now.  I really doubt we'll get above that
uint64_t IDCounter = 0;

void e_InitEntities()
{
    entityArena = MakeDataArena(Entity, 1, 4);
}

// TODO: Add check for deleted entities (will also make deleted entity ID linked list at some point)
uint64_t e_CreateEntity()
{
    Entity* e =  ar_AllocOne(entityArena);
    versor q;
    glmc_mat4_identity(e->transform);
    glmc_mat4_quat(e->transform, q);
    e->ID = IDCounter;
    e->parent = UINT64_MAX;
    IDCounter++;
    idArr[e->ID] = e;
    return e->ID;
}

// TODO: Add check to see if parent exists
int e_CreateEntityWithParent(uint64_t parentID)
{
    uint64_t ent = e_CreateEntity();
    Entity *e = (Entity*)idArr[ent];
    e->parent = parentID;
    return ent;
}

mat4* e_GetEntityTransform(uint64_t id)
{
    Entity* en = (Entity*)idArr[id];
    return en->transform;
}