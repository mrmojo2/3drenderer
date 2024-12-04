#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "vector.h"

#define CUBE_DETAIL 9
#define N_POINTS (CUBE_DETAIL*CUBE_DETAIL*CUBE_DETAIL)
#define FOV_FACTOR 640

vec3_t camera_position = {.x = 0, .y=0, .z=5};
vec3_t cube_rotation = {.x = 0, .y=0, .z=0};

vec3_t cube_points[N_POINTS];
vec2_t projected_cube_points[N_POINTS];

bool is_running = false;
int previous_frame_time=0;

void setup(void){
	//allocate required memory to hold color buffer
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t)*window_width*window_height);
	
	
	//Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	//loading array of vectors of cube
	int count = 0;

	/*
	float step =2.0f/(CUBE_DETAIL-1);
	for(float x = -1;x<=1;x+=step){
		for(float y = -1;y<=1;y+=step){
			for(float z = -1;z<=1;z+=step){
				vec3_t point = {x,y,z};
				cube_points[count++] = point;
			}
		}
	}*/


	float step =2.0f/(CUBE_DETAIL-1);
	for(int i=0;i<CUBE_DETAIL;i++){
		for(int j=0;j<CUBE_DETAIL;j++){
			for(int k=0;k<CUBE_DETAIL;k++){
				float x = -1 + i*(step);
				float y = -1 + j*(step);
				float z = -1 + k*(step);
				vec3_t point = {x,y,z};
				cube_points[count++] = point;
			}
		}
	}
}

void process_input(void){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type){
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE)
				is_running=false;
			break;
	}
}

//3d to 2d (orthographic projection)
vec2_t project(vec3_t point){
	vec2_t result = {
		.x = (FOV_FACTOR*point.x)/point.z,
		.y = (FOV_FACTOR*point.y)/point.z
		};
	return result;
}

void update(void){
	//control the fps of the game loop
	
	/*while(!SDL_TICKS_PASSED(SDL_GetTicks(),previous_frame_time+FRAME_TARGET_TIME)){
		//waste time
	}*/
	// while loop uses 100% cpu core!! beter to use delay function provided by sdl
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
		SDL_Delay(time_to_wait);
	}
	
	previous_frame_time = SDL_GetTicks();            //how may ms since SDL_INIT
	

	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	cube_rotation.z += 0.01;

	for(int i=0;i < N_POINTS; i++){
		vec3_t point = cube_points[i];
		
		//transform the point
		vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

		//move the points away from camera
		transformed_point.z -= camera_position.z;
		
		//project point
		projected_cube_points[i] = project(transformed_point);
	}
}


void render(void){
	//draw_grid(10);

	//loop all projected points and render them
	for(int i=0;i<N_POINTS;i++){
		vec2_t point = projected_cube_points[i];
		draw_rect(
			point.x+(window_width/2),
			point.y+(window_height/2),
			4,4,
			0xffffff00
		);
	}
	
	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);                  //This swaps the back buffer (where drawing happens) with the front buffer (displayed on the screen), making the rendered image visible to the user
}


int main(void){
	is_running = initialize_window();
	
	setup();

	while(is_running){
		process_input();
		update();
		render();
	}
	
	destroy_window();
	
	return 0;
}
