/*******************************************************************************************
*	Common structures needed by the program
*
*   Created by Davis Teigeler
********************************************************************************************/
#ifndef structsh
#define structsh
#include "memarena.h"
#include <cglm/call.h>

//-----------------------------------------------------------------------------
//{ Shader Definitions 
//-----------------------------------------------------------------------------

typedef struct
{
	unsigned int ID;
} Shader;

typedef struct
{
	Shader s;
	uint32_t texref;	// The gpu id of the relevant texture array
	Arena	*models;	// The memory locations of all our models within the texture group;
	uint32_t modelCt;
} ShaderGroup;


//-----------------------------------------------------------------------------
//} End Shader Definitions 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//{ Mesh Data Definitions 
//-----------------------------------------------------------------------------

typedef struct MeshData
{
	float 	*vertices;			// Contains position, TexCoordinates, Normals, etc
	uint32_t vertCount;
	uint32_t *indices;			// Index Order for the EBO
	uint32_t indexCount;
} MeshData;


//-----------------------------------------------------------------------------
//} End Mesh Data Definitions 
//-----------------------------------------------------------------------------

typedef struct Ray
{
	vec3 origin;
	vec3 dir;
} Ray;




















#endif