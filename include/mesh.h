#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6*2) //6 cube faces, 2 triangles per face

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];


//same arrays as above but made more generic using dynamic arrays and struct
//a mesh_t type is an object that is to be rendered
typedef struct {
	vec3_t* vertices;    //dynamic array of 3d vertices
	face_t* faces;       //dynamic array of faces
	vec3_t rotation;     //angle of the mesh object in x y and z direction (in radians) 
} mesh_t;

extern mesh_t mesh;


void load_cube_to_mesh(void);
void load_obj_to_mesh(FILE* f);

#endif
