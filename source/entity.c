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
    /* versor q;
    glmc_mat4_identity(e->transform);
    glmc_mat4_quat(e->transform, q); */
    /* e->position = {0,0,0};
    e->rotation = {0,0,0};
    e->scale = {0,0,0}; */
    glmc_vec3_one(e->position);
    glmc_vec3_zero(e->rotation);
    glmc_vec3_one(e->scale);
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
    versor q;
    glmc_mat4_identity(en->transform);
    glmc_mat4_quat(en->transform, q);
    glmc_translate(en->transform, en->position);
    glmc_rotate_x(en->transform, en->rotation[0], en->transform);
    glmc_rotate_y(en->transform, en->rotation[1], en->transform);
    glmc_rotate_z(en->transform, en->rotation[2], en->transform);
    glmc_scale(en->transform, en->scale);
    return en->transform;
}

void e_SetEnitityPosition(uint64_t id, vec3 pos)
{
    Entity* en = (Entity*)idArr[id];
    en->position[0] = pos[0];
    en->position[1] = pos[1];
    en->position[2] = pos[2];
}
void e_SetEnitityRotation(uint64_t id, vec3 rot)
{
    Entity* en = (Entity*)idArr[id];
    en->rotation[0] = rot[0];
    en->rotation[1] = rot[1];
    en->rotation[2] = rot[2];
}
void e_SetEnitityScale(uint64_t id, vec3 scale)
{
    Entity* en = (Entity*)idArr[id];
    en->scale[0] = scale[0];
    en->scale[1] = scale[1];
    en->scale[2] = scale[2];
}