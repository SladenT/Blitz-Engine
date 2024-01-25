/*******************************************************************************************
*	Shader wrapper loading and functions
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "glad/glad.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// should setup DEBUG vs RELEASE versions
#define SHADER_PATH "../res/shaders/"
//#define SHADER_PATH "res/shaders/" // RELEASE VERSION

char* ReadFromFile(const char* filename);

void sh_UseShader(unsigned int ID)
{
	glUseProgram(ID);
}

void sh_SetBool(const char* name, Shader s, bool value)
{
	glUniform1i(glGetUniformLocation(s.ID, name), (int)value);
}

void sh_SetInt(const char* name, Shader s, int value)
{
	glUniform1i(glGetUniformLocation(s.ID, name), value);
}

void sh_SetFloat(const char* name, Shader s, float value)
{
	glUniform1f(glGetUniformLocation(s.ID, name), value);
}

Shader sh_BuildShader(const char* vShader, const char* fShader)
{
	Shader s = {glCreateProgram()};
	char vsBuffer[128];
	char fsBuffer[128];
	strncpy(vsBuffer, SHADER_PATH, 64);
	strncpy(fsBuffer, SHADER_PATH, 64);
	strncat(vsBuffer, vShader, 64);
	strncat(fsBuffer, fShader, 64);
	const char* vs = ReadFromFile(vsBuffer);
	const char* fs = ReadFromFile(fsBuffer);
	
	//Compile Vertex Shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs, NULL);
	glCompileShader(vertexShader);
	
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf(infoLog);
	}
	success=0;
	
	//Compile Fragment Shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs, NULL);
	glCompileShader(fragmentShader);
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf(infoLog);
	}
	success=0;
	
	//Link Shader
	glAttachShader(s.ID, vertexShader);
	glAttachShader(s.ID, fragmentShader);
	glLinkProgram(s.ID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glGetProgramiv(s.ID, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(s.ID, 512, NULL, infoLog);
		printf(infoLog);
	}
	//free((char*)vs);
	//free((char*)fs);
	return s;
}



char* ReadFromFile(const char* filename)
{
	FILE *f = fopen(filename, "r");
	char *s;
	long size=0;
	if (f == NULL)
	{
		perror("Cannot open file");
		return NULL;
	}
	else
	{
		// Get our file size
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		rewind(f);
	}
	// it should be pure plain text so we need to allocate the whole block of bytes.
	s = malloc(size);
	
	int i = 0;
	while (!feof(f))
	{
		s[i] = getc(f);
		i++;
	}
	return s;
}
















