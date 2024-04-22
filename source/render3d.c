/*******************************************************************************************
 *	Rendering methods and pipeline.  Meshes are logically ordered into groups, and similar
 *  Meshes should be placed in similar groups.
 *
 *  Created by Davis Teigeler
 ********************************************************************************************/
// #include <assimp/cimport.h>

#include "render3d.h"
#include "camera.h"
#include "entity.h"
#include "material.h"
#include "resimport.h"
#include "collider.h"
#include "physics.h"
#include <cglm/call.h>
#include <stdio.h>
#include "stb_image.h"

#define SHADER_GROUP_MAX_COUNT 128

unsigned int SCR_WIDTH = 960;
unsigned int SCR_HEIGHT = 540;

Camera* c;

unsigned int texture;

ShaderGroup shaderGroups[SHADER_GROUP_MAX_COUNT];
int shaderCount = 0;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
unsigned int depthMapFBO;
unsigned int depthMap;
Shader depthShadowShader;
vec3 lightPos = (vec3) {2.0f, 2.0f, -4.0f};

GLFWwindow *mainWindowContext;

// Test box

/* static float vertices[] = {
	
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // A
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // B
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // A
	
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // F
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // H
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // H
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // D
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // H

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // G
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // F
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // B
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // B
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // G

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // B
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // F
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // F
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // E
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // A

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // G
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // H
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f // G
}; */

// #region Callback Definitions
static void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
// #endregion

// #region Forward Declarations
void r3d_GenerateMeshOne(Model *mod, float vertexData[], int vertAttCount,  int indexData[], int indexCount);
Model* r3d_GenerateModelOne(float vertexData[], int vertAttCount, int indexData[], int indexCount, Shader s, uint64_t entID, uint32_t matID);
// #endregion

GLFWwindow* r3d_getMainWindow()
{
    return mainWindowContext;
}

unsigned int r3d_GetScreenWidth()
{
    return SCR_WIDTH;
}

unsigned int r3d_GetScreenHeight()
{
    return SCR_HEIGHT;
}

GLFWwindow* r3d_InitWindowRender(void)
{
    if (!glfwInit())
    {
        // We busted
        return 0;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    mainWindowContext = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShepRTS", NULL, NULL);
    if (!mainWindowContext)
    {
        // Window or OpenGL context creation failed
        printf("Window broke");
    }
    glfwMakeContextCurrent(mainWindowContext);
    // Setup the function bindings for later versions of openGL using GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // VSYNC
    glfwSwapInterval(1);
    // Back Face Culling (Counter Clockwise Order)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // Enable Z-Buffer
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Setup callback
    glfwSetErrorCallback(error_callback);
    glfwSetFramebufferSizeCallback(mainWindowContext, FramebufferSizeCallback);

    // Temp for test display
    /* Shader s = sh_BuildShader("def.vs", "def.fs");
    uint64_t ent = e_CreateEntity();
    r3d_GenerateModelOne(vertices, 180, s, ent, mat_CreateDefaultMaterial(0, ent, false)); */
    c = cam_GetMainCamera();

    // Depth Texture for Shadow Mapping
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    depthShadowShader = sh_BuildShader("shadowmap.vs", "shadowmap.fs");

    return mainWindowContext;
}

Model* r3d_GenerateModelOne(float vertexData[], int vertAttCount, int indexData[], int indexCount, Shader s, uint64_t entID, uint32_t matID)
{
    if (shaderCount == 0)
    {
        shaderGroups[0].s = s;
        // TODO: Get texture array for this shader
        shaderGroups[0].models = MakeDataArena(Model, 1, 4);
        shaderGroups[0].modelCt = 1;
        Model *m = ar_AllocOne(shaderGroups[0].models);
        r3d_GenerateMeshOne(m, vertexData, vertAttCount, indexData, indexCount);
        versor q;
        glmc_mat4_identity(m->transform);
        glmc_mat4_quat(m->transform, q);
        shaderCount++;
        m->mat = matID;
        m->ID = entID;
        return m;
    }
    else 
    {
        for (int i = 0; i < shaderCount; i++)
        {
            if (s.ID == shaderGroups[i].s.ID)
            {
                shaderGroups[i].modelCt++;
                Model *m = ar_AllocOne(shaderGroups[0].models);
                r3d_GenerateMeshOne(m, vertexData, vertAttCount, indexData, indexCount);
                versor q;
                glmc_mat4_identity(m->transform);
                glmc_mat4_quat(m->transform, q);
                m->mat = matID;
                m->ID = entID;
                return m;
            }
        }
        // New shader, add to shader Count
        shaderGroups[shaderCount].s = s;
        // TODO: Get texture array for this shader
        shaderGroups[shaderCount].models = MakeDataArena(Model, 1, 4);
        shaderGroups[shaderCount].modelCt = 1;
        Model *m = ar_AllocOne(shaderGroups[shaderCount].models);
        r3d_GenerateMeshOne(m, vertexData, vertAttCount, indexData, indexCount);
        versor q;
        glmc_mat4_identity(m->transform);
        glmc_mat4_quat(m->transform, q);
        shaderCount++;
        m->mat = matID;
        m->ID = entID;
        return m;
    }
    // TODO: Add in support for adding more shaders to the shaderGroup Array;
    return NULL;
}

Model* r3d_GenerateFromMeshData(MeshData data, Shader s, uint64_t entID, uint32_t matID)
{   
    Model* m = r3d_GenerateModelOne(data.vertices, data.vertCount*8, data.indices, data.indexCount, s, entID, matID);
    m->vertexCount = data.indexCount;
    return m;
}

void r3d_GenerateMeshOne(Model *mod, float vertexData[], int vertAttCount,  int indexData[], int indexCount)
{
    /* uint32_t VAO;  //Vertex Array Object, which stores our VBO and EBO
	uint32_t VBO;  //Vertex Buffer Object, which stores our vertex/texture/misc. data for the shader to use
	uint32_t EBO; */
    glGenVertexArrays(1, &mod->mesh.VAO);
    glGenBuffers(1, &mod->mesh.VBO);
    glGenBuffers(1, &mod->mesh.EBO);
    glBindVertexArray(mod->mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mod->mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertAttCount*sizeof(float), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mod->mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(int), indexData, GL_STATIC_DRAW);

    //Verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //UV Coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    //Vertex Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void r3d_RenderPass(GLFWwindow* window, double deltaTime)
{
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // TEMPORARY
    float gValue = 0.5f;
    float rValue = 0.5f;
    float bValue = 0.5f;
    mat4 camTransform;
    cam_GetCamTransform(*c, camTransform);
    vec3 outPos;
    cam_GetCamPosition(*c, outPos);

    // END TEMPORARY
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

    // Shadow Mapping
    Ray ray = c_GetCameraRay(*c, SCR_WIDTH/2, SCR_HEIGHT/2);
    float dist = p_CheckRaycastDist(ray, 0X00000002); // Ground mask
    float dirX = ray.dir[0] * dist;
    float dirY = ray.dir[1] * dist;
    float dirZ = ray.dir[2] * dist;
    vec3 lookAtPos = {ray.origin[0] + dirX, 0, ray.origin[2] + dirZ};
    vec3 lightPosition = {lookAtPos[0]-2, 4, lookAtPos[2] - 1};

    mat4 lightProjection, lightView, lightSpaceMatrix;
    float near_plane = -10.0f-(outPos[1]), far_plane = 30.5f+(outPos[1]/6);
    glmc_ortho(-12.0f*(outPos[1]/3), 12.0f*(outPos[1]/3), -12.0f*(outPos[1]/3), 12.0f*(outPos[1]/3), near_plane, far_plane, lightProjection);
    vec3 camPos;
    cam_GetCamPosition(*c, camPos);
    glmc_lookat(lightPosition, (vec3) {lookAtPos[0], 0, lookAtPos[2]}, (vec3) {0.0, 1.0, 0.0}, lightView);
    glmc_mat4_mul(lightProjection, lightView, lightSpaceMatrix);

    // OLD SHADOW MAP
    /* mat4 lightProjection, lightView, lightSpaceMatrix;
    float near_plane = -10.0f*(outPos[1]/10), far_plane = 30.5f*(outPos[1]/8);
    glmc_ortho(-10.0f*outPos[1], 10.0f*outPos[1], -10.0f*outPos[1], 10.0f*outPos[1], near_plane, far_plane, lightProjection);
    vec3 lightOffset;
    vec3 camPos;
    cam_GetCamPosition(*c, camPos);
    glmc_vec3_add(lightPos, camPos, lightOffset);
    lightOffset[1] = 4;
    lightOffset[0] -= 11;
    glmc_lookat(lightOffset, (vec3) {-7.0f + camPos[0], 0, -3.0f + camPos[2]}, (vec3) {0.0, 1.0, 0.0}, lightView);
    glmc_mat4_mul(lightProjection, lightView, lightSpaceMatrix); */

    vec3 lightDir;
    glmc_euler_angles(lightView, lightDir);
    
    sh_UseShader(depthShadowShader.ID);
    int lsMatrixLoc = glGetUniformLocation(depthShadowShader.ID, "lightSpaceMatrix");
    int s_vertexTransformLoc = glGetUniformLocation(depthShadowShader.ID, "transform");
    glUniformMatrix4fv(lsMatrixLoc, 1, GL_FALSE, (float *)lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < shaderCount; i++)
    {
        for (int j = 0; j < shaderGroups[i].modelCt; j++)
        {
            Model* model = ar_ArenaIterator(shaderGroups[i].models, &j);
            glUniformMatrix4fv(s_vertexTransformLoc, 1, GL_FALSE, (float *)e_GetEntityTransform(model->ID));
            glBindVertexArray(model->mesh.VAO);
            glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Regular Render Pass
    for (int i = 0; i < shaderCount; i++)
    {
        sh_UseShader(shaderGroups[i].s.ID);
        int vertexColorLocation = glGetUniformLocation(shaderGroups[i].s.ID, "ourColor");
        int vertexTransformLoc = glGetUniformLocation(shaderGroups[i].s.ID, "transform");
        int vertexViewLoc = glGetUniformLocation(shaderGroups[i].s.ID, "view");
        int vertexProjectionLoc = glGetUniformLocation(shaderGroups[i].s.ID, "proj");
        int cameraPosLoc = glGetUniformLocation(shaderGroups[i].s.ID, "viewPos");
        int lightPosLoc = glGetUniformLocation(shaderGroups[i].s.ID, "lightPos");
        int lightSpaceMatrixLoc = glGetUniformLocation(shaderGroups[i].s.ID, "lightSpaceMatrix");

        glUniformMatrix4fv(vertexProjectionLoc, 1, GL_FALSE, (float *)c->projection);
        glUniform4f(vertexColorLocation, rValue, gValue, bValue, 1.0f);
        glUniformMatrix4fv(vertexViewLoc, 1, GL_FALSE, (float *)camTransform);
        glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, (float *)lightSpaceMatrix);
        glUniform1i(glGetUniformLocation(shaderGroups[i].s.ID, "texArray"), 1);
        glUniform1i(glGetUniformLocation(shaderGroups[i].s.ID, "shadowMap"), 0);
        glUniform3f(glGetUniformLocation(shaderGroups[i].s.ID, "lightDirectional"), lightDir[0], lightDir[1], lightDir[2]);
        

        glUniform3f(lightPosLoc, lightPosition[0], lightPosition[1], lightPosition[2]);
        //glUniform3f(lightPosLoc, lightOffset[0], lightOffset[1], lightOffset[2]);

        
        glUniform3f(cameraPosLoc, outPos[0], outPos[1], outPos[2]);

        // Texture Array 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, res_GetTexArray0());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        for (int j = 0; j < shaderGroups[i].modelCt; j++)
        {
            Model* model = ar_ArenaIterator(shaderGroups[i].models, &j);
            // TODO: modify transform by parent transforms
            glUniformMatrix4fv(vertexTransformLoc, 1, GL_FALSE, (float *)e_GetEntityTransform(model->ID));
            glBindVertexArray(model->mesh.VAO);
            // Process materials for the model per shader
            mat_SetShaderFromMaterial(mat_GetMatFromID(model->mat), shaderGroups[i].s);
            //glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
            glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    //glfwSwapBuffers(window);
}
