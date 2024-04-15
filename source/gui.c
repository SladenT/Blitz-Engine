/*******************************************************************************************
*	Basic gui 
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
#include <nuklear/style.c>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

struct nk_glfw glfw = {0};
struct nk_context* ctx;
GLFWwindow* win;
struct nk_font_atlas *atlas;
int width1, height1;
 
void gui_Init(GLFWwindow* window)
{
    win = window;
    ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);

    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);
 
}

void gui_Render()
{
    glfwPollEvents();
    nk_glfw3_new_frame(&glfw);

    gui_admin();
    gui_settings();
    
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
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
            {
                fprintf(stdout, "button pressed\n");
            }

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
    //size of initial window: 960,540
    if (nk_begin(ctx, "Settings", nk_rect(730, 5, 225, 200), NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE))
        {
            //repositions gui when window size changes
            nk_window_set_position(ctx, "Settings", nk_vec2(get_width() - 230 , 5));

            //Volume slider
            static float value = 0.6f;
            nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
                {
                    nk_layout_row_push(ctx, 50);
                    nk_label(ctx, "Volume:", NK_TEXT_LEFT);
                    nk_layout_row_push(ctx, 110);
                    nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
                 }
            nk_layout_row_end(ctx);

            //Theme selector
            enum {BLACK, BLUE, DARK, RED, WHITE};
            static int current_Theme = RED;
            nk_layout_row_dynamic(ctx, 30, 2);
            if(nk_option_label(ctx, "red", current_Theme == RED))
            {
                current_Theme = RED;
                set_style(ctx, THEME_RED);
            } 
            if(nk_option_label(ctx, "blue", current_Theme == BLUE)) 
            {
                current_Theme = BLUE;
                set_style(ctx, THEME_BLUE);
            } 
            if(nk_option_label(ctx, "dark", current_Theme == DARK)) 
            {
                current_Theme = DARK;
                set_style(ctx, THEME_DARK);
            } 
            if(nk_option_label(ctx, "black", current_Theme == BLACK)) 
            {
                current_Theme = BLACK;
                set_style(ctx, THEME_BLACK);
            } 
            if(nk_option_label(ctx, "white", current_Theme == WHITE)) 
            {
                current_Theme = WHITE;
                set_style(ctx, THEME_WHITE);
            } 
            

        }
    nk_end(ctx);
}

get_height()
{
    glfwGetWindowSize(win, &width1, &height1);
    return height1;
}

get_width()
{
    glfwGetWindowSize(win, &width1, &height1);
    return width1;
}