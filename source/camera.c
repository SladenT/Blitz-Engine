/*******************************************************************************************
*	Basic camera functionality.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "camera.h"
#include "utility.h"

static bool init = false;
Camera* mainCam;
float defaultAspectRatio = 1920.0f/1080.0f;

#define CAM_MAX 50.0f
#define CAM_MIN 6.0f

static void InitializeMainCamera(float aspectRatio);

// CameraLerp
bool doMove;
vec3 startPos;
vec3 endPos;
float distance = 0;
float speed = 0;
float t = 0;

Camera* cam_GetMainCamera(void)
{
    if (!init)
    {
        init = true;
        InitializeMainCamera(defaultAspectRatio);
    }
    return mainCam;
}

void cam_GetCamPosition(Camera cam, vec3 outPos)
{
    outPos[0] = cam.position[0];
    outPos[1] = cam.position[1];
    outPos[2] = cam.position[2];
}

void cam_GetCamTransform(Camera cam, mat4 view)
{
    vec3 cameraUp = {0.0f, 1.0f,  0.0f};
    glmc_mat4_identity(view);
    vec3 c;
    glmc_vec3_add(cam.position, cam.rotation, c);
    glmc_lookat(cam.position, c, cameraUp, view);
}

void cam_TranslateCameraBy(Camera *cam, vec3 pos)
{
    cam->position[0] += pos[0];
    cam->position[1] += pos[1];
    cam->position[2] += pos[2];
}

void cam_BeginLerp(vec3 start, vec3 end, float speedF, double deltaTime)
{
    if (doMove)
    {
        float dir1 = startPos[1] - endPos[1];
        float dir2 = start[1] - end[1];
        if ((dir1 < 0 && dir2 < 0) || (dir1 > 0 && dir2 > 0))
        {
            if (dir1 < 0)
            {
                endPos[1] += 0.5f*(speed/2);
            }
            else
            {
                endPos[1] -= 0.5f*(speed/2);
            }
            t = u_InvLerp(startPos[1], endPos[1], mainCam->position[1]);
            distance = glmc_vec3_distance(startPos, endPos);
            speed += speedF;
            return;
        }
    }
    glmc_vec3_copy(start, startPos);
    glmc_vec3_copy(end, endPos);
    distance = glmc_vec3_distance(startPos, endPos);
    speed = speedF;
    t = 0;
    doMove = true;
}
void cam_LerpLoop(double deltaTime)
{
    if (doMove && t < 1.0f)
    {
        if ((mainCam->position[1] >= CAM_MIN) && (mainCam->position[1] <= CAM_MAX))
        {
            float value = u_Lerp(startPos[1], endPos[1], t);
            mainCam->position[1] = value;
            t += (deltaTime/distance) * speed*2;
        }
        else
        {
            if ( mainCam->position[1] < CAM_MIN)
            {
                mainCam->position[1] = CAM_MIN;
            }
            else
            {
                mainCam->position[1] = CAM_MAX;
            }
            t = 0;
            doMove = false;
        }
        
    }
    else if (t >= 1.0f)
    {
        t = 0;
        doMove = false;
    }
    else
    {
        doMove = false;
    }
}

static void InitializeMainCamera(float aspectRatio)
{
    mainCam = malloc(sizeof(Camera));
    // Initial View
    /* mat4 view;
	glmc_mat4_identity(view);
    vec3 cameraPos   = {0.0f, 2.0f,  3.0f};
	vec3 cameraFront = {0.0f, -0.5f,  -1.0f};
	vec3 cameraUp    = {0.0f, 1.0f,  0.0f};
    vec3 c;
	glmc_vec3_add(cameraPos, cameraFront, c);
	glmc_lookat(cameraPos, c, cameraUp, view); */
	
	mat4 projection;
	glmc_mat4_identity(projection);
	glmc_perspective(glm_rad(60.0f), aspectRatio, 0.1f, 10000.0f, projection);
    
    //glmc_mat4_copy(view, mainCam->transform);
    glmc_mat4_copy(projection, mainCam->projection);
    mainCam->aspectRatio = aspectRatio;
    mainCam->position[0] = 0.0f;
    mainCam->position[1] = 2.0f;
    mainCam->position[2] = 3.0f;
    mainCam->rotation[0] = 0.0f;
    mainCam->rotation[1] = -glm_rad(60.0f);
    mainCam->rotation[2] = -1.0f;
}

