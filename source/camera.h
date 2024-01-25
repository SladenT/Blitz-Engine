#ifndef camerah
#define camerah
#include "common.h"
#include <cglm/call.h>

typedef struct 
{
	mat4 transform;
    mat4 projection;
    float aspectRatio;
} Camera;


Camera* cam_GetMainCamera(void);






















#endif