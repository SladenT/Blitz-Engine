/*******************************************************************************************
*	General resource import handler.  Only supports importing images, but it should eventually
*   Support 3D meshes, audio, databases, etc.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "resimport.h"
#include "components.h"
#include <assimp/cimport.h>        
#include <assimp/scene.h>          
#include <assimp/postprocess.h>    
#include <stb_image.h>
#include <glad/glad.h>
#include <dirent.h>

#ifdef DEBUG
#define TEX_FOLDER "../res/textures/"
#endif
#ifndef DEBUG
#define TEX_FOLDER "/res/textures/"
#endif

void res_InitImport()
{
    // Texture setup
    stbi_set_flip_vertically_on_load(true);
    // For each subfolder within the textures folder, create a texture array
    unsigned int texture;
    glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    //printf( "glTexStorage3D = %p", glTexStorage3D );
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 512, 512, 256);
    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelC, GL_RGBA8, wid, hei, layerC);
	// Texture Parameters (wrapping, mipmaps, etc.)
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   /*  glBindTexture(GL_TEXTURE_2D, texture);
	// Texture Parameters (wrapping, mipmaps, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
    
    DIR *d = opendir(TEX_FOLDER);
    struct dirent* dir;
    while ((dir = readdir(d)) != NULL)
    {
        if (strncmp(dir->d_name, ".", 259)  == 0){continue;}
        if (strncmp(dir->d_name, "..", 259)  == 0){continue;}
        char buffer[260];
        strncpy(buffer, TEX_FOLDER, 259);
        strncat(buffer, dir->d_name, 259);
        DIR *tex1 = opendir(buffer);
        struct dirent* texdir;
        uint32_t count = 0;
        while ((texdir = readdir(tex1)) != NULL)
        {
            if (strncmp(texdir->d_name, ".", 259)  == 0){continue;}
            if (strncmp(texdir->d_name, "..", 259)  == 0){continue;}
            char imbuffer[260];
            strncpy(imbuffer, buffer, 259);
            strncat(imbuffer, "/", 259);
            strncat(imbuffer, texdir->d_name, 259);
            int width, height, nrChannels;
	        unsigned char *data = stbi_load(imbuffer, &width, &height, &nrChannels, 4);
            if (data)
            {
                //glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                //glGenerateMipmap(GL_TEXTURE_2D);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,count,width,height,1,GL_RGBA,GL_UNSIGNED_BYTE,data);
                //glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RGB,width,height,count,0, GL_RGB, GL_UNSIGNED_BYTE, data);
                //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
                //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                count++;
            }
            stbi_image_free(data);
        }
        closedir(tex1);
    }
    closedir(d);
    

	/* // Load image into memory
	int width, height, nrChannels;
	unsigned char *data = stbi_load("../res/textures/arr0/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Image Failed to Load!");
	}
	stbi_image_free(data); */

}

MeshData res_ImportMesh(const char *pFile)
{
    const struct aiScene* scene = aiImportFile( pFile,
                                                aiProcess_Triangulate            |
                                                aiProcess_JoinIdenticalVertices  |
                                                aiProcess_SortByPType);
    MeshData md = (MeshData){0};
    if (scene == NULL) 
    {
        printf("ERROR: Could not import mesh asset");
        return md;
    }
    //struct aiNode *node = scene->mRootNode;
    struct aiMesh* mesh = scene->mMeshes[0];
    float *mData = malloc((sizeof(float)*5)*mesh->mNumVertices);
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        int index = i*5;
        // Verts
        mData[index] = mesh->mVertices[i].x;
        mData[index+1] = mesh->mVertices[i].y;
        mData[index+2] = mesh->mVertices[i].z;
        // TexCoords
        mData[index+3] = mesh->mTextureCoords[0][i].x;
        mData[index+4] = mesh->mTextureCoords[0][i].y;
    }
    int *indexOrder = malloc(sizeof(int)*(mesh->mNumFaces * 3));
    md.indexCount = 0;
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indexOrder[(i*3)+j] = face.mIndices[j];
            md.indexCount++;    
        }    
    }
    md.indices = indexOrder;
    md.vertCount = mesh->mNumVertices;
    md.vertices = mData;
    aiReleaseImport(scene);
    return md;
    //printf("%d", node->mNumMeshes);
}