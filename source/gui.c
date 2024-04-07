/*******************************************************************************************
*	Basic gui initialization
*	Work in progress
*
*   Created by Evan Posharow
********************************************************************************************/
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

struct nk_glfw glfw = {0};
struct nk_context* ctx;
GLFWwindow* win;
struct nk_font_atlas *atlas;
 
void gui_Init(GLFWwindow* window)
{
    win = window;
    ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);

    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);

    
}

void gui_Render()
{
        // GUI
    //if (nk_begin(ctx, "GUI Version 1", nk_rect(50, 50, 230, 250), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    glfwPollEvents();
    nk_glfw3_new_frame(&glfw);

    gui_admin();
    gui_settings();
    
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glfwSwapBuffers(win);
}

void gui_admin()
{
    if (nk_begin(ctx, "GUI Version 1", nk_rect(0, 0, 230, 250), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "toggle 1", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "toggle 2", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Volume:", 0, &property, 100, 10, 1);
            
            }
    nk_end(ctx);
}

void gui_settings()
{
    
    if (nk_begin(ctx, "Settings", nk_rect(730, 5, 225, 100), NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE))
        {
            static float value = 0.6f;

            nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
                {
                    nk_layout_row_push(ctx, 50);
                    nk_label(ctx, "Volume:", NK_TEXT_LEFT);
                    nk_layout_row_push(ctx, 110);
                    nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
                 }
            nk_layout_row_end(ctx);

        }
    nk_end(ctx);
}