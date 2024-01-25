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
	Shader s;
	uint32_t texref;	// The gpu id of the relevant texture array
	Arena	*models;	// The memory locations of all our models within the texture group;
	uint32_t modelCt;
} ShaderGroup;


//-----------------------------------------------------------------------------
//} End Shader Definitions 
//-----------------------------------------------------------------------------






















#endif