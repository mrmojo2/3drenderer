#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                DIFFERENT OPTIONS FOR RENDERING
///////////////////////////////////////////////////////////////////////////////////////////////////////
int SCALE_FACTOR       = 640;
bool BACK_FACE_CULLING = false;
bool OUTLINE_TRIANGLES = true;
bool FILL_TRIANGLES    = false;
bool COLOR_VERTEX      = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//                GLOBALS
//////////////////////////////////////////////////////////////////////////////////////////////////////
triangle_t* triangles_to_render = NULL;
vec3_t camera_position = {0,0,0};
bool is_running = false;
int previous_frame_time=0;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                 FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void setup(char* obj_path){
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
	
	FILE *f = fopen(obj_path,"r"); 
	if(f == NULL){
		//loads default cube to mesh
		load_cube_to_mesh();
	
	}else{
		//parse obj file and load object to mesh
		load_obj_to_mesh(f);
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
			if(event.key.keysym.sym == SDLK_1){
				OUTLINE_TRIANGLES = true;
				FILL_TRIANGLES    = false;
				COLOR_VERTEX      = true;
			}
			if(event.key.keysym.sym == SDLK_2){
				OUTLINE_TRIANGLES = true;
				FILL_TRIANGLES    = false;
				COLOR_VERTEX      = false;
			}
			if(event.key.keysym.sym == SDLK_3){
				OUTLINE_TRIANGLES = false;
				FILL_TRIANGLES    = true;
				COLOR_VERTEX      = false;
			}
			if(event.key.keysym.sym == SDLK_4){
				OUTLINE_TRIANGLES = true;
				FILL_TRIANGLES    = true;
				COLOR_VERTEX     = false;
			}
			if(event.key.keysym.sym == SDLK_c){
				BACK_FACE_CULLING = true;
			}
			if(event.key.keysym.sym == SDLK_d){
				BACK_FACE_CULLING = false;
			}

			break;
	}
}

//3d to 2d (perspective projection)
vec2_t project(vec3_t point){
	vec2_t result = {
		.x = (SCALE_FACTOR*point.x)/point.z,
		.y = (SCALE_FACTOR*point.y)/point.z
		};
	return result;
}

void update(void){
	//control the fps of the game loop
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);         // while loop uses 100% cpu core!! beter to use delay function provided by sdl
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
		SDL_Delay(time_to_wait);
	}
	
	previous_frame_time = SDL_GetTicks();            //how may ms since SDL_INIT

	//initialize the dynamic arrray of triangles to render
	triangles_to_render = NULL;

	mesh.rotation.x += 0.02;
	mesh.rotation.y += 0.02;
	mesh.rotation.z += 0.00;

	//loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for(int i=0;i<num_faces;i++){
		face_t mesh_face = mesh.faces[i];
		
		vec3_t face_vertices[3]; 
		face_vertices[0] = mesh.vertices[mesh_face.a-1];
		face_vertices[1] = mesh.vertices[mesh_face.b-1];
		face_vertices[2] = mesh.vertices[mesh_face.c-1];	
		
		vec3_t transformed_vertices[3];


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//               loop all three vertices of this current face and apply transformations
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for(int j=0;j<3;j++){
			vec3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
			
			//translate vertex away from the camera
			transformed_vertex.z += 5;
			
			//save transformation in the array transformed_vertices
			transformed_vertices[j] = transformed_vertex;
		}

		if(BACK_FACE_CULLING){
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//                     perform backface culling
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		vec3_t A = transformed_vertices[0]; /*  A  */
		vec3_t B = transformed_vertices[1]; /* / \ */
		vec3_t C = transformed_vertices[2]; /* C-B */
		
		vec3_t normal = vec3_cross(vec3_sub(B,A),vec3_sub(C,A)); /* in right handed coordinate system do cross-product in opposite order....i think */

		vec3_t camera_ray = vec3_sub(camera_position,A);

		//check alignment of normal with camera ray using dot product
		float alignment = vec3_dot(normal,camera_ray);

		if(alignment < 0) continue;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		//                    loop all three vertices and perfrom projection
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		vec2_t projected_vertices[3];
		
		for(int j =0 ; j < 3; j++){	
			//project current vertex
			projected_vertices[j] = project(transformed_vertices[j]);
			
			//scale and translate the projecte points to the middle of screen
			projected_vertices[j].x += (window_width / 2);
			projected_vertices[j].y += (window_height / 2);

		}

		//calculate the avg depth of each face based on the vertices after transformation
		float avg_z = (transformed_vertices[0].z+transformed_vertices[1].z+transformed_vertices[2].z)/3.0;

		triangle_t projected_triangle = {
			.points = {
				{projected_vertices[0].x, projected_vertices[0].y},
				{projected_vertices[1].x, projected_vertices[1].y},
				{projected_vertices[2].x, projected_vertices[2].y},
			},

			.color = mesh_face.color,
			.avg_depth = avg_z

		};

		//save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);	
		
	}

	//Sort the triangls to rendered by their avg_depth
	int num_triangles = array_length(triangles_to_render);
	for(int i=0;i < num_triangles; i++){
		for(int j=i ; j < num_triangles; j++){
			if(triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth){
				triangle_t tmp = triangles_to_render[i];
				triangles_to_render[i] = triangles_to_render[j];
				triangles_to_render[j] = tmp;
			}
		}
	}
}


void render(void){
	//draw_grid(10);

	//loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for(int i=0;i<num_triangles;i++){
		triangle_t triangle = triangles_to_render[i];
		
		if(FILL_TRIANGLES){
			fill_triangle(triangle,triangle.color);
		}
		if(OUTLINE_TRIANGLES){
			draw_triangle(triangle,0xff808080);
		}
		if(COLOR_VERTEX){
			for(int i=0 ; i < 3; i++){
				vec2_t vertex = triangle.points[i];
				draw_rect(vertex.x, vertex.y,5,5,0xffff0000);
			}
		}

	}

	//clear the array of triangles to render every frame loop
	array_free(triangles_to_render);

	
	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);                  //This swaps the back buffer (where drawing happens) with the front buffer (displayed on the screen), making the rendered image visible to the user
}

void free_resources(){
	array_free(mesh.faces);
	array_free(mesh.vertices);
	free(color_buffer);
}


int main(int argc, char **argv){

	//parse command line arguments
	int option;
	while((option = getopt(argc,argv,"s:bf"))!=-1){
		switch(option){
			case 's':
				SCALE_FACTOR = atoi(optarg);
				break;
			case 'b':
				BACK_FACE_CULLING = true;
				break;
			case 'f':
				FILL_TRIANGLES = true;
				break;
			case '?':
				fprintf(stderr,"unknown option: -%c\n",optopt);
				return 1;
		}
	}





	///////////////////////////////////////////////////////////////////////
	//                             GAME LOOP                            //
	//////////////////////////////////////////////////////////////////////
	is_running = initialize_window();
	
	setup(argv[optind]);

	while(is_running){
		process_input();
		update();
		render();
	}
	
	destroy_window();
	free_resources();
	return 0;
}