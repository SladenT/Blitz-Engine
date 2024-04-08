#ifndef camerah
#define camerah
#include "common.h"
#include <cglm/call.h>

typedef struct 
{
	//mat4 transform;
    vec3 position;
    vec3 rotation;
    mat4 projection;
    float aspectRatio;
} Camera;


Camera* cam_GetMainCamera(void);
void cam_GetCamPosition(Camera cam, vec3 outPos);
void cam_TranslateCameraBy(Camera *cam, vec3 pos);
void cam_GetCamTransform(Camera cam, mat4 view);






















#endif