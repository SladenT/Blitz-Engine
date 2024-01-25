/*******************************************************************************************
*	Various component structures to be used by various systems throughout the program.
*	Entities are defined by ID, and thus need no actual structure, only a logical reference
*
*   Created by Davis Teigeler
********************************************************************************************/
#ifndef componentsh
#define componentsh
#include "cglm/call.h"
#include "common.h"

//-----------------------------------------------------------------------------
//{ Model Definitions 
//-----------------------------------------------------------------------------
typedef struct 
{
	uint32_t VAO;  //Vertex Array Object, which stores our VBO and EBO
	uint32_t VBO;  //Vertex Buffer Object, which stores our vertex/texture/misc. data for the shader to use
	uint32_t EBO;  //Element Buffer Object (Index Order for Tris)
} Mesh;

typedef struct __attribute__((__packed__)) // 120 Bytes (it doesn't properly pack mesh otherwise)
{
	mat4 	  transform;  
	Mesh 	  mesh;	  
	uint32_t  textures[8];  // An array of references to our various textures.
							// To reduce texture bind calls, we'll group our models by the 0th index of this array.
	uint32_t  texCount;		// Lets us know the size of our texture array.  Can be 0, for no textures.
	uint64_t  ID;		 	// Entity ID
} Model;


//-----------------------------------------------------------------------------
//} End Model Definitions 
//-----------------------------------------------------------------------------

























#endif