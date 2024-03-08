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
#include <nuklear/nuklear.h>

void gui_Start()
{
    struct nk_context ctx;
    struct nk_font_atlas atlas;

    //set font to default font
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);

    nk_init_default(&ctx, &atlas.default_font->handle);
    

   //nk_begin(&ctx,"Show", nk_rect(50, 50, 220, 220), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE);
}