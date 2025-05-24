#include "display.h"
#include "triangle.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
float* inv_z_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Eror Initializing SDL \n");
        return false;
    }

    // use sdl to query the full screen size of the display
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    // Create SDL Window
    window = SDL_CreateWindow(
        NULL, // no window name
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        fprintf(stderr, "Error Creating SDL Window\n");
        return false;
    }

    // Create SDL Renderer
    renderer = SDL_CreateRenderer(
        window,
        -1, // use default() output/display device
        0   // no special flags
    );

    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL Renerer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); // Change video mode to be real fullscreen from fake fullscreen

    return true;
}

void draw_grid(int size)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            if (x % size == 0 || y % size == 0)
                color_buffer[(window_width * y) + x] = 0xFF333333;
        }
    }
}

void draw_point_grid(int size)
{
    for (int y = 0; y < window_height; y += size)
    {
        for (int x = 0; x < window_width; x += size)
        {
            if (x % size == 0 || y % size == 0)
                color_buffer[(window_width * y) + x] = 0xFFFFFFFF;
        }
    }
}

void draw_rect(int pos_x, int pos_y, int w, int h, uint32_t color)
{
    for (int y = pos_y; y < pos_y + h; y++)
    {
        for (int x = pos_x; x < pos_x + w; x++)
        {
            draw_pixel(x,y,color);
        }
    }
}

void draw_pixel(int pos_x, int pos_y, uint32_t color){
	if(pos_x < window_width && pos_x>=0 && pos_y < window_height && pos_y>=0) color_buffer[(window_width * pos_y) + pos_x] = color;
}

void clear_color_buffer(uint32_t color)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}
void clear_inv_z_buffer(void)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            inv_z_buffer[(window_width * y) + x] = 0.0;		//in our coord z value grows from 0 to 1 ie the farthest objct has z value 1 
        }
    }
}

/*drawing line using dda algorithm
void draw_line(int x0, int y0, int x1, int y1,uint32_t color){
	int delta_x = x1-x0;
	int delta_y = y1-y0;

	//taking the longest in rise run to draw the full line
	int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);
	
	float x_inc = delta_x / (float) side_length;
	float y_inc = delta_y / (float) side_length;

	float current_x = x0;
	float current_y = y0;

	for(int i=0;i<=side_length;i++){
		draw_pixel(round(current_x),round(current_y),color);
		current_x += x_inc;
		current_y += y_inc;
	}
}*/

//drawing line using Bresenham's algorithm
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x0, y0, color); // Draw the current pixel

        if (x0 == x1 && y0 == y1) break; // End condition

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}



void render_color_buffer()
{

    /*copies the pixel data from color_buffer (a raw array of pixel values) to color_buffer_texture. Essentially, it converts raw pixel data into a format that SDL can work with (i.e., a texture). */
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t)));

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL); // Copies a texture to the renderer's target (prepares the frame).
}

void destroy_window()
{
    // reverse order of allocation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
