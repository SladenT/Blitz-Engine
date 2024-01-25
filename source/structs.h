/*******************************************************************************************
*	Common structures needed by the program
*
*   Created by Davis Teigeler
********************************************************************************************/
#ifndef structsh
#define structsh
#include "memarena.h"

//-----------------------------------------------------------------------------
//{ Shader Definitions 
//-----------------------------------------------------------------------------

typedef struct
{
	unsigned int ID;
} Shader;

typedef struct
{
	uint32_t texref;	// The gpu id of the relevant texture
	Arena	*models;	// The memory locations of all our models within the texture group;
} TextureGroup;

typedef struct
{
	Shader s;
	Arena *tgroups;	    // The memory locations of all the texture groups within the shader group
} ShaderGroup;


//-----------------------------------------------------------------------------
//} End Shader Definitions 
//-----------------------------------------------------------------------------






















#endif