/*******************************************************************************************
*	Creation and handling of all GUIs
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
int width1, height1;
struct nk_font *menuFont;
struct nk_font *basicFont;
 
void gui_Init(GLFWwindow* window)
{
    //size of initial window: 960,540
    win = window;
    ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);

    //adding custom fonts
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    menuFont = nk_font_atlas_add_from_file(atlas, "../include/nuklear/extra_font/trade-winds.regular.ttf", 50, 0);
    basicFont = nk_font_atlas_add_from_file(atlas, "../include/nuklear/extra_font/proggy-clean.ttf", 18, 0);
    nk_glfw3_font_stash_end(&glfw);
    nk_style_set_font(ctx, &menuFont->handle);
    
    }
 
}

void gui_Render()
{
    glfwPollEvents();
    nk_glfw3_new_frame(&glfw);

    //creates all guis
    gui_admin();
    gui_settings();
    gui_buildings();
    gui_main_menu();
    

    
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void gui_admin()
{
    int INITIAL_GUI_X_POSITION = 0;
    int INITIAL_GUI_Y_POSITION = 0;
    int INITIAL_GUI_WIDTH = 260;
    int INITIAL_GUI_HEIGHT = 250;
    if (nk_begin(ctx, "GUI Version 1", nk_rect(INITIAL_GUI_X_POSITION, INITIAL_GUI_Y_POSITION, INITIAL_GUI_WIDTH, INITIAL_GUI_HEIGHT), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
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
    int INITIAL_GUI_X_POSITION = 730;
    int INITIAL_GUI_Y_POSITION = 5;
    int INITIAL_GUI_WIDTH = 225;
    int INITIAL_GUI_HEIGHT = 200;
    if (nk_begin(ctx, "Settings", nk_rect(INITIAL_GUI_X_POSITION, INITIAL_GUI_Y_POSITION, INITIAL_GUI_WIDTH, INITIAL_GUI_HEIGHT), NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE))
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

            //Theme selector toggles
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

void gui_buildings()
{
    // to do: add nk_rect as constants
    //        add curency and gui for currency
    int INITIAL_GUI_X_POSITION = 240;
    int INITIAL_GUI_Y_POSITION = 432;
    int INITIAL_GUI_WIDTH = 480;
    int INITIAL_GUI_HEIGHT = 108;
    if (nk_begin(ctx, "Build", nk_rect(INITIAL_GUI_X_POSITION, INITIAL_GUI_Y_POSITION, INITIAL_GUI_WIDTH, INITIAL_GUI_HEIGHT), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE))
        {   
            //scale position with window
            int dynamicX = (get_width()/2) - (get_width()/4);
            int dynamicY = get_height() - 108;
            nk_window_set_position(ctx, "Build", nk_vec2(dynamicX , dynamicY));

            //scale size with window
            nk_window_set_size(ctx,"Build", nk_vec2(get_width()/2 , 108));

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Price:", NK_TEXT_ALIGN_CENTERED);

            //4 labels for build menu
            nk_layout_row_dynamic(ctx, 20, 4);
            nk_label_colored(ctx, "100 Gold", NK_TEXT_ALIGN_CENTERED, nk_rgb(240, 252, 3));
            nk_label_colored(ctx, "200 Wood",   NK_TEXT_ALIGN_CENTERED, nk_rgb(0, 255, 0));
            nk_label(ctx, "idk", NK_TEXT_ALIGN_CENTERED);
            nk_label(ctx, "zero", NK_TEXT_ALIGN_CENTERED);

            //4 buttons for build menu
            nk_layout_row_dynamic(ctx, 36, 4);
            if (nk_button_label(ctx, "Buy Units"))
            {
                fprintf(stdout, "\nBought a unit");
            }
            if (nk_button_label(ctx, "Build Tree"))
            {
                fprintf(stdout, "\nBuilt a Tree");
            }
            if (nk_button_label(ctx, "testing 1"))
            {
                fprintf(stdout, "\nstill testing 1");
            }
            if (nk_button_label(ctx, "testing 2"))
            {
                fprintf(stdout, "\nstill testing 2");
            }
            

        }
    nk_end(ctx);
}

void gui_main_menu()
{
    int INITIAL_GUI_X_POSITION = 0;
    int INITIAL_GUI_Y_POSITION = 0;
    int INITIAL_GUI_WIDTH = 960;
    int INITIAL_GUI_HEIGHT = 540;
    if (nk_begin(ctx, "Menu", nk_rect(INITIAL_GUI_X_POSITION, INITIAL_GUI_Y_POSITION, INITIAL_GUI_WIDTH, INITIAL_GUI_HEIGHT), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE))
        {   
            //scale size to whole screen
            nk_window_set_size(ctx,"Menu", nk_vec2(get_width() , get_height()));

            //initially hides all gui when main menu is open
            nk_window_show(ctx, "GUI Version 1", nk_false);
            nk_window_show(ctx, "Settings", nk_false);
            nk_window_show(ctx, "Build", nk_false);

            nk_spacer(ctx);
            nk_layout_row_dynamic(ctx, 100, 1);

            nk_label_colored(ctx, "RTS Game Demo", NK_TEXT_ALIGN_CENTERED, nk_rgb(255, 165, 0));
            nk_label_colored(ctx, "Made Using Our Custom Blitz Engine!", NK_TEXT_ALIGN_CENTERED, nk_rgb(255, 165, 0));
            nk_label_colored(ctx, "By: Davis Teigeler, Evan Posharow, and Aaron Hartle", NK_TEXT_ALIGN_CENTERED, nk_rgb(255, 255, 255));
            if (nk_button_label(ctx, "Start demo"))
            {
                //shows all guis again
                nk_window_show(ctx, "GUI Version 1", nk_true);
                nk_window_show(ctx, "Settings", nk_true);
                nk_window_show(ctx, "Build", nk_true);

                //change font ant then hides main menu
                nk_style_set_font(ctx, &basicFont->handle);
                nk_window_set_focus(ctx, "Build");
                nk_window_show(ctx, "Menu", nk_false);
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