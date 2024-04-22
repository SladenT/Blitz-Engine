# Blitz Engine

This is the capstone project for Shepherd University students Davis Teigeler, Aaron Hartle, and Evan Posharow.  It is an engine dedicated towards Real Time Strategy games, created using OpenGL 4.2, mostly in C.

## How to use it?
You can import all of the source code into your own project (removing the main.c and game.c files) in order to use it as a fully editable engine, or you can build the files into a library and import it that way.

In order to setup the engine, you must call these functions in order
```C
e_InitEntities();
GLFWwindow* window = r3d_InitWindowRender();
res_InitImport();
gui_Init(window);
p_PhysicsInit();
debug_Init(); // Optional
```

After which, you set up the render loop, which would look like this:
```C
while (!glfwWindowShouldClose(window))
{
    double deltaTime = glfwGetTime() - lastTime;
    lastTime = glfwGetTime();
    p_PhysicsUpdate(deltaTime);
    r3d_RenderPass(window, deltaTime);
    gui_Render();
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

This will create your window and begin the engine functions.  You can then use the various libraries (such as entity.h, resimport.h, and camera.h) to create entities, import models and move the camer around.

## Demo Build Instructions

1. Install MinGW (this was setup using version 12.2, but the latest should work), to ensure you get the gcc compiler.  Make sure MinGW and its executables are added to the PATH
2. Add the folder to VSCode. (Make sure you have the C/C++ official Windows extension for VSCode)
3. Create a "bin" subdirectory and copy the glfw3.dll, glm.dll and assimp-vc142-mtd.dll from lib into that folder
4. Run it using the play button

## Prerequisites

This project requires the Visual C Redistributable, at least 2019, to build and run.  It has been tested on Windows 7 and Windows 11.

3254 lines of original code (as of commit 60)