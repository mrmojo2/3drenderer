#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "options.h"

#define M_PI 3.141592

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                DIFFERENT OPTIONS FOR RENDERING
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool BACK_FACE_CULLING = false;
bool OUTLINE_TRIANGLES = true;
bool FILL_TRIANGLES    = false;
bool TEXTURE_TRIANGLES    = false;
bool COLOR_VERTEX      = false;
bool SHADING	       = false;
bool FLAT_SHADING      = true;
bool GOUROUD_SHADING  = true;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                GLOBALS
//////////////////////////////////////////////////////////////////////////////////////////////////////
triangle_t* triangles_to_render = NULL;
vec3_t camera_position = {0,0,0};
bool is_running = false;
int previous_frame_time=0;
mat4_t projection_matrix;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                 FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////
void update(void);

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

	//initialize the perspective projection matrix
	float fov    = M_PI/3.0;
	float aspect = (float ) window_height / (float)window_width;
	float znear  = 0.1; 
	float zfar   = 100.0;
	printf("height: %d, width: %d\n", window_height, window_width);
	projection_matrix = get_perspective_projection_matrix(aspect,fov,zfar,znear);
	
	//load objfile or the default cube to mesh object
	FILE *f = fopen(obj_path,"r"); 
	if(f == NULL){
		//loads default cube to mesh
		load_cube_to_mesh();
	
	}else{
		//parse obj file and load object to mesh
		load_obj_to_mesh(f);
	}

	//load texture file
	load_png_texture("./assets/textures/doge.png");
}

void process_input(void){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type){
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE){
				is_running = false;
			}
			if(event.key.keysym.sym == SDLK_c){
				BACK_FACE_CULLING = !BACK_FACE_CULLING;
			}
			if(event.key.keysym.sym == SDLK_s){
				SHADING = !SHADING; 
			}
			if(event.key.keysym.sym == SDLK_f){
				FILL_TRIANGLES = !FILL_TRIANGLES;
			}
			if(event.key.keysym.sym == SDLK_o){
				OUTLINE_TRIANGLES = !OUTLINE_TRIANGLES;
			}
			if(event.key.keysym.sym == SDLK_t){
				TEXTURE_TRIANGLES = !TEXTURE_TRIANGLES;
			}

			break;
	}
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

	//rotate and scale the mesh object
	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;
	
	//mesh.rotation.x  = -M_PI/4;

	//mesh.scale.x    += 0.002;
	//mesh.scale.y    += 0.001;

	//mesh.translation.x += 0.01;
	mesh.translation.z = 5;			//move points away from camera


	//create scale ,rotation and transformation matrices
	mat4_t scale_matrix       = get_scale_matrix(mesh.scale.x, mesh.scale.y, mesh.scale.y);
	mat4_t rotation_matrix_x  = get_rotation_matrix_x(mesh.rotation.x); 	
	mat4_t rotation_matrix_y  = get_rotation_matrix_y(mesh.rotation.y); 
	mat4_t rotation_matrix_z  = get_rotation_matrix_z(mesh.rotation.z); 
	mat4_t translation_matrix = get_translation_matrix(mesh.translation.x, mesh.translation.y,mesh.translation.z); 
	
	//loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for(int i=0;i<num_faces;i++){
		face_t mesh_face = mesh.faces[i];
		
		vec3_t face_vertices[3]; 
		face_vertices[0] = mesh.world_vertices[mesh_face.a-1];
		face_vertices[1] = mesh.world_vertices[mesh_face.b-1];
		face_vertices[2] = mesh.world_vertices[mesh_face.c-1];	
		
		vec4_t transformed_vertices[3];

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//               loop all three vertices of this current face and apply transformations
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for(int j=0;j<3;j++){
			vec4_t transformed_vertex = get_vec4(face_vertices[j]);
			
		     	//perfom transformation of vertices using matrices
			mat4_t world_matrix = mat4_identity();
			
			world_matrix = mat4_mul_mat4(scale_matrix,world_matrix);		//NOTE:always follow the order scale,rotate,translate			
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);		//as we are rotating about our coordinate axes so must rotate before translation
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);	
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix,world_matrix);	
			
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

			//save transformation in the array transformed_vertices
			transformed_vertices[j] = transformed_vertex;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//                     perform backface culling and flat shading
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		vec3_t A = get_vec3(transformed_vertices[0]); /*  A  */
		vec3_t B = get_vec3(transformed_vertices[1]); /* / \ */
		vec3_t C = get_vec3(transformed_vertices[2]); /* C-B */
		
		vec3_t normal = vec3_cross(vec3_sub(B,A),vec3_sub(C,A)); /* in right handed coordinate system do cross-product in opposite order....i think */
		mesh.faces[i].normal = normal; 				//saving the normal for each face
		vec3_t camera_ray = vec3_sub(camera_position,A);
		
		vec3_t unit_normal = vec3_get_unit(normal);
		vec3_t unit_camera_ray = vec3_get_unit(camera_ray);
		vec3_t unit_light      = vec3_get_unit(light.direction);

		//check alignment of normal with camera ray and light ray using dot product
		float alignment_with_camera = vec3_dot(unit_normal,unit_camera_ray);
		float alignment_with_light = vec3_dot(unit_normal,unit_light);

		if(BACK_FACE_CULLING){
			if(alignment_with_camera < 0) continue;
		}
		if(SHADING){
			if(FLAT_SHADING)
				mesh_face.color = light_apply_intensity(mesh_face.color, 1-(alignment_with_light+1)/2);
		}
		

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		//                    loop all three vertices and perfrom projection
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		vec4_t projected_vertices[3];
		
		for(int j =0 ; j < 3; j++){	
			//project current vertex
			projected_vertices[j] = vec4_project(projection_matrix,transformed_vertices[j]);
			
			//scale and translate the projecte points to the middle of screen
			projected_vertices[j].x *= (window_width/2.0);
			projected_vertices[j].y  *= (window_height/2.0);
			
			//flip y as direction of y in normal coordinates and screen coordinates is opposite
			projected_vertices[j].y *= -1;


			projected_vertices[j].x += (window_width / 2.0);
			projected_vertices[j].y += (window_height / 2.0);
		}

		//calculate the avg depth of each face based on the vertices after transformation
		float avg_z = (transformed_vertices[0].z+transformed_vertices[1].z+transformed_vertices[2].z)/3.0;

		triangle_t projected_triangle = {
			.face = &(mesh.faces[i]),
			.points = {
				{projected_vertices[0].x, projected_vertices[0].y,projected_vertices[0].z,projected_vertices[0].w},
				{projected_vertices[1].x, projected_vertices[1].y,projected_vertices[1].z,projected_vertices[1].w},
				{projected_vertices[2].x, projected_vertices[2].y,projected_vertices[2].z,projected_vertices[2].w},
			},

			.color = mesh_face.color,
			.avg_depth = avg_z,
			.texcoords = {
				{mesh_face.a_uv.u, mesh_face.a_uv.v},
				{mesh_face.b_uv.u, mesh_face.b_uv.v},
				{mesh_face.c_uv.u, mesh_face.c_uv.v}
			},
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
			fill_triangle(&triangle,triangle.color);
		}
		if(TEXTURE_TRIANGLES){
			draw_textured_triangle(&triangle,texture);
		}
		if(OUTLINE_TRIANGLES){
			draw_triangle(triangle,0xff808080);
		}
		if(COLOR_VERTEX){
			for(int i=0 ; i < 3; i++){
				vec4_t vertex = triangle.points[i];
				draw_rect(vertex.x, vertex.y,5,5,0xffff0000);
			}
		}

	}
	/*for(int i=0;i<texture_width;i++){
		for(int j = 0 ; j < texture_height ; j++){
			draw_pixel(i,j,texture[texture_width * i + j]);
		}
	}*/
	
	/*vec3_t n = {1,2,3};
	face_t f = {
		.a = 1, .b = 2, .c = 3,
		.normal = n
	};
	triangle_t test = {
		.face = &f,
		{{600,900,1,1},{100,200,1,1},{200,650,1,1}},
		{{0,1},{0,0},{1,0}},
		0xffff00ff,
		69

	};
	if(FILL_TRIANGLES){
		fill_triangle(&test,test.color);
	}
	if(TEXTURE_TRIANGLES){
		draw_textured_triangle(&test,texture);
	}
	if(OUTLINE_TRIANGLES){
		draw_triangle(test,0xff808080);
	}
	if(COLOR_VERTEX){
		for(int i=0 ; i < 3; i++){
			vec4_t vertex =test.points[i];
			draw_rect(vertex.x, vertex.y,5,5,0xffff0000);
		}
	}*/



	//clear the array of triangles to render every frame loop
	array_free(triangles_to_render);

	
	render_color_buffer();
	clear_color_buffer(0xFFaabbcc);

	SDL_RenderPresent(renderer);                  //This swaps the back buffer (where drawing happens) with the front buffer (displayed on the screen), making the rendered image visible to the user
}

void free_resources(){
	array_free(mesh.faces);
	array_free(mesh.world_vertices);
	array_free(mesh.transformed_vertices);
	free_neighbor_list();
	free(color_buffer);
	free(texture);
}


int main(int argc, char **argv){

	//parse command line arguments
	int option;
	char *s_value = NULL;
	while((option = getopt(argc,argv,"s:"))!=-1){
		switch(option){
			case 's':
				s_value = optarg;
				if(s_value){
					if(strcmp(s_value,"f") == 0){
						FLAT_SHADING = true;			
						GOUROUD_SHADING =false;			
					}else if (strcmp(s_value,"g") == 0){
						FLAT_SHADING = false;			
						GOUROUD_SHADING = true;			
					
					}else {
					
					}
				}
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

	update();
	generate_vertex_neighbor();

	while(is_running){
		process_input();
		update();
		render();
	}
	
	destroy_window();
	free_resources();
	return 0;
}
