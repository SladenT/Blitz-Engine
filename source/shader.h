#ifndef shaderh
#define shaderh
// Runs our shader
void sh_UseShader(unsigned int ID);

// Compiles and creates a shader program using the source file names
Shader sh_BuildShader(const char* vShader, const char* fShader);

void sh_SetInt(const char* name, Shader s, int value);
void sh_SetFloat(const char* name, Shader s, float value);



































#endif