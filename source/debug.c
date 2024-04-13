/*******************************************************************************************
*	Various debug functions, mostly visualizations
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "debug.h"
#include "shader.h"

Shader debugShader;

void debug_Init(void)
{
    debugShader = sh_BuildShader("debug.vs", "debug.fs");
}

void debug_DrawRay(Ray r)
{   
    //glDisable(GL_CULL_FACE);
    Camera* c = cam_GetMainCamera();
    mat4 view;
    cam_GetCamTransform(*c, view);
    float raydat[] = {r.origin[0], r.origin[1], r.origin[2], 
                      r.origin[0]+(r.dir[0]*5000), r.origin[1]+(r.dir[1]*5000), r.origin[2] + (r.dir[2]*5000)};
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(raydat), raydat, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    sh_UseShader(debugShader.ID);
    
    int vertexViewLoc = glGetUniformLocation(debugShader.ID, "view");
    int vertexProjectionLoc = glGetUniformLocation(debugShader.ID, "proj");
    int colorLoc = glGetUniformLocation(debugShader.ID, "color");

    glUniformMatrix4fv(vertexProjectionLoc, 1, GL_FALSE, (float *)c->projection);
    glUniformMatrix4fv(vertexViewLoc, 1, GL_FALSE, (float *)view);
    glUniform4f(colorLoc, 1.0, 0.0, 0.0, 1.0f);

    glBindVertexArray(VAO);

    glDrawArrays(GL_LINES, 0, 3);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    //glEnable(GL_CULL_FACE);
}