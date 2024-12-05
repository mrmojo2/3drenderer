#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mesh.h"
#include "array.h"

#define LINE_BUFFER 1024

mesh_t mesh = {
	.vertices   = NULL,
	.faces      = NULL,
	.rotation   = {0,0,0},
	.scale      = {1.0,1.0,1.0},
	.translation = {0,0,0}
};

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
    {.a = 1, .b = 2, .c = 3, .color = 0xffff0000 },
    {.a = 1, .b = 3, .c = 4, .color = 0xffff0000},
    // right
    {.a = 4, .b = 3, .c = 5, .color = 0xff00ff00},
    {.a = 4, .b = 5, .c = 6, .color = 0xff00ff00},
    // back
    {.a = 6, .b = 5, .c = 7, .color = 0xff0000ff},
    {.a = 6, .b = 7, .c = 8, .color = 0xff0000ff},
    // left
    {.a = 8, .b = 7, .c = 2, .color = 0xffffff00},
    {.a = 8, .b = 2, .c = 1, .color = 0xffffff00},
    // top
    {.a = 2, .b = 7, .c = 5, .color = 0xffff00ff},
    {.a = 2, .b = 5, .c = 3, .color = 0xffff00ff},
    // bottom
    {.a = 6, .b = 8, .c = 1, .color = 0xff00ffff},
    {.a = 6, .b = 1, .c = 4, .color = 0xff00ffff},
};


void load_cube_to_mesh(void){
	for(int i=0;i<N_CUBE_VERTICES;i++){
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices,cube_vertex);
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
			array_push(mesh.vertices,vertex);
			//printf("v %f %f %f\n",vertex.x, vertex.y, vertex.z);
		}
		else if (strncmp(line_buffer, "f ", 2) == 0)
		{
			/* NOTE: the obj file has no color value but our face_t structure requires it.... since we have not supplied face.color it takes random garbage value */
			face_t face;

			int matches = sscanf(line_buffer, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &face.a,&face.b,&face.c);
			if(matches < 3) {
				matches = sscanf(line_buffer, "f %d %d %d", &face.a,&face.b,&face.c);
			}
			//printf("f %d %d %d\n",face.a, face.b, face.c); 
			array_push(mesh.faces, face);
		}
	}
}
