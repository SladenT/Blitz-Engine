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
#include <cglm/call.h>
#include <stdio.h>
#include "stb_image.h"

#define SHADER_GROUP_MAX_COUNT 128

unsigned int SCR_WIDTH = 960;
unsigned int SCR_HEIGHT = 540;

//Shader s;
Camera* c;

unsigned int texture;

ShaderGroup shaderGroups[SHADER_GROUP_MAX_COUNT];
int shaderCount = 0;

// Test box


/* float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};   */

static float vertices[] = {
	
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
};

// #region Callback Definitions
static void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShepRTS", NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        printf("Window broke");
    }
    glfwMakeContextCurrent(window);
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
    // Setup callback
    glfwSetErrorCallback(error_callback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Temp for test display
    /* Shader s = sh_BuildShader("def.vs", "def.fs");
    uint64_t ent = e_CreateEntity();
    r3d_GenerateModelOne(vertices, 180, s, ent, mat_CreateDefaultMaterial(0, ent, false)); */
    c = cam_GetMainCamera();
    return window;
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
        return m;
    }
    // TODO: Add in support for more than one mesh being capable of generated
    return NULL;
}

Model* r3d_GenerateFromMeshData(MeshData data, Shader s, uint64_t entID, uint32_t matID)
{
    return r3d_GenerateModelOne(data.vertices, data.vertCount, data.indices, data.indexCount, s, entID, matID);
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount, indexData, GL_STATIC_DRAW);

    //Verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //UV Coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void r3d_RenderPass(GLFWwindow* window, double deltaTime)
{
    // TEMPORARY
    

    float gValue = 0.5f;
    float rValue = 0.5f;
    float bValue = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        glmc_translate(c->transform, (vec3){0.0f,0.0f,1.0 * deltaTime});
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glmc_translate(c->transform, (vec3){0.0f,0.0f,-1.0 * deltaTime});
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        glmc_translate(c->transform, (vec3){1.0 * deltaTime,0.0f,0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        glmc_translate(c->transform, (vec3){-1.0 * deltaTime,0.0f,0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        glmc_translate(c->transform, (vec3){0.0f,-1.0 * deltaTime,0.0f});
    }

    // END TEMPORARY
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < shaderCount; i++)
    {
        sh_UseShader(shaderGroups[i].s.ID);
        int vertexColorLocation = glGetUniformLocation(shaderGroups[i].s.ID, "ourColor");
        int vertexTransformLoc = glGetUniformLocation(shaderGroups[i].s.ID, "transform");
        int vertexViewLoc = glGetUniformLocation(shaderGroups[i].s.ID, "view");
        int vertexProjectionLoc = glGetUniformLocation(shaderGroups[i].s.ID, "proj");

        glUniformMatrix4fv(vertexProjectionLoc, 1, GL_FALSE, (float *)c->projection);
        glUniform4f(vertexColorLocation, rValue, gValue, bValue, 1.0f);
        glUniformMatrix4fv(vertexViewLoc, 1, GL_FALSE, (float *)c->transform);
        glUniform1i(glGetUniformLocation(shaderGroups[i].s.ID, "texArray"),0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 1);

        for (int i = 0; i < shaderGroups[i].modelCt; i++)
        {
            Model* model = ar_ArenaIterator(shaderGroups[i].models, &i);
            // TODO: modify transform by parent transforms
            glUniformMatrix4fv(vertexTransformLoc, 1, GL_FALSE, (float *)e_GetEntityTransform(model->ID));
            glBindVertexArray(model->mesh.VAO);
            // TODO: Process materials for the model per shader
            mat_SetShaderFromMaterial(mat_GetMatFromID(model->mat), shaderGroups[i].s);
            // TODO: Need to get the actual vertex count at some point, or change to EBO...
            //glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
            glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
}
