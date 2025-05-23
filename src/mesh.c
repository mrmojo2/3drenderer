#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mesh.h"
#include "array.h"

#define LINE_BUFFER 1024

mesh_t mesh = {
	.vertex_neighbor_list = NULL,
	.world_vertices   = NULL,
	.transformed_vertices   = NULL,
	.faces      = NULL,
	.rotation   = {0,0,0},
	.scale      = {1.0,1.0,1.0},
	.translation = {0,0,0}
};


////////////////////////////////////////////////////////////////////
//	Default cube
////////////////////////////////////////////////////////////////////

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y =  1, .z = -1}, // 2
    {.x =  1, .y =  1, .z = -1}, // 3
    {.x =  1, .y = -1, .z = -1}, // 4
    {.x =  1, .y =  1, .z =  1}, // 5
    {.x =  1, .y = -1, .z =  1}, // 6
    {.x = -1, .y =  1, .z =  1}, // 7
    {.x = -1, .y = -1, .z =  1}  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .color = 0xffff0000, .a_uv = {0,0}, .b_uv = {0,1}, .c_uv = {1,1}},
    {.a = 1, .b = 3, .c = 4, .color = 0xffff0000, .a_uv = {0,0}, .b_uv = {1,1}, .c_uv = {1,0}},
    // right
    {.a = 4, .b = 3, .c = 5, .color = 0xff00ff00, .a_uv = {0,0}, .b_uv = {0,1}, .c_uv = {1,1}},
    {.a = 4, .b = 5, .c = 6, .color = 0xff00ff00, .a_uv = {0,0}, .b_uv = {1,1}, .c_uv = {1,0}},
    // back
    {.a = 6, .b = 5, .c = 7, .color = 0xff0000ff, .a_uv = {0,0}, .b_uv = {0,1}, .c_uv = {1,1}},
    {.a = 6, .b = 7, .c = 8, .color = 0xff0000ff, .a_uv = {0,0}, .b_uv = {1,1}, .c_uv = {1,0}},
    // left
    {.a = 8, .b = 7, .c = 2, .color = 0xffffff00, .a_uv = {0,0}, .b_uv = {0,1}, .c_uv = {1,1}},
    {.a = 8, .b = 2, .c = 1, .color = 0xffffff00, .a_uv = {0,0}, .b_uv = {1,1}, .c_uv = {1,0}},
    // top
    {.a = 2, .b = 7, .c = 5, .color = 0xffff00ff, .a_uv = {0,0}, .b_uv = {0,1}, .c_uv = {1,1}},
    {.a = 2, .b = 5, .c = 3, .color = 0xffff00ff, .a_uv = {0,0}, .b_uv = {1,1}, .c_uv = {1,0}},
    // bottom
    {.a = 6, .b = 8, .c = 1, .color = 0xff00ffff, .a_uv = {0,0}, .b_uv = {0,1}, .c_uv = {1,1}},
    {.a = 6, .b = 1, .c = 4, .color = 0xff00ffff, .a_uv = {0,0}, .b_uv = {1,1}, .c_uv = {1,0}},
};


void load_cube_to_mesh(void){
	for(int i=0;i<N_CUBE_VERTICES;i++){
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.world_vertices,cube_vertex);
	}
	for(int i=0;i<N_CUBE_FACES;i++){
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces,cube_face);
	}
}

void string_n_get(char *main_string, const char *delim,int n, char *splitted){
	char *token = strtok(main_string, delim);

	for(int i=0;i<n-1;i++){
		token = strtok(NULL,delim);
		if(token == NULL){
			splitted[0] = '\0';
			return;
		}
	}
	strcpy(splitted,token);
}



void load_obj_to_mesh(FILE *f)
{
	char line_buffer[LINE_BUFFER];
	while (fgets(line_buffer, LINE_BUFFER, f))
	{
		if (strncmp(line_buffer, "v ", 2) == 0)
		{
			vec3_t vertex;	
			sscanf(line_buffer,"v %f %f %f",&vertex.x,&vertex.y,&vertex.z);
			array_push(mesh.world_vertices,vertex);
			//printf("v %f %f %f\n",vertex.x, vertex.y, vertex.z);
		}
		else if (strncmp(line_buffer, "f ", 2) == 0)
		{
			/* NOTE: the obj file has no color value but our face_t structure requires it.... since we have not supplied face.color it takes random garbage value */
			face_t face;
			face.color = 0xff00ccaa;
			int matches = sscanf(line_buffer, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &face.a,&face.b,&face.c);
			if(matches < 3) {
				matches = sscanf(line_buffer, "f %d %d %d", &face.a,&face.b,&face.c);
			}
			
			vec3_t n = {0,0,0};
			face.normal = n;
			//printf("f %d %d %d\n",face.a, face.b, face.c); 
			array_push(mesh.faces, face);
		}
	}
}

void generate_vertex_neighbor(void){
	int num_vertices = array_length(mesh.world_vertices);
	int num_faces = array_length(mesh.faces);
	printf("num vertices: %d ",num_vertices);
	printf("num faces   : %d  \n",num_faces);

	for(int i=0; i< num_vertices; i++){
		int vertex_index = i+1; 		//vertex index starts from 1 in obj file
		face_t** neighbors = NULL;
		for(int j = 0; j < num_faces; j++){
			face_t* current_face = &mesh.faces[j];

			if(current_face->a == vertex_index || current_face->b == vertex_index || current_face->c == vertex_index){
				array_push(neighbors,current_face);
			}
		}
		array_push(mesh.vertex_neighbor_list, neighbors);
	}
	
	int s = array_length(mesh.vertex_neighbor_list);
	for(int i=0; i<s; i++){
		int is = array_length(mesh.vertex_neighbor_list[i]);
		for(int j = 0; j<is; j++){
			face_t* current = ((face_t **)mesh.vertex_neighbor_list[i])[j];
			printf("(%d,%d,%d)  ",current->a,current->b,current->c);
		}
		printf("\n");
	}
}

void free_neighbor_list(void){
	int size = array_length(mesh.vertex_neighbor_list);
	for(int i = 0; i < size; i++){
		array_free(mesh.vertex_neighbor_list[i]);
	}
	array_free(mesh.vertex_neighbor_list);
}
