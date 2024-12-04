#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "triangle.h"

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                      

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;
extern int window_width;
extern int window_height;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                   FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool initialize_window(void);

void draw_grid(int size);
void draw_point_grid(int size);
void draw_pixel(int x,int y, uint32_t color);
void draw_rect(int pos_x, int pos_y, int w, int h, uint32_t color);
void draw_line(int x0,int y0,int x1, int y1,uint32_t color);


void clear_color_buffer(uint32_t color);
void render_color_buffer();
void destroy_window();

#endif
