#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct{			//Fun Fact: Translation is NOT a linear transformation but a shear is
	float m[4][4];		//so we raise an extra dimention and perform shear to get translation in original dimention
} mat4_t;			//this is why we use 4x4 matrices to transform our 3d vertices


mat4_t mat4_identity(void);

mat4_t get_scale_matrix(float sx, float sy, float sz);
mat4_t get_translation_matrix(float tx, float ty, float tz);
mat4_t get_rotation_matrix_z(float angle);
mat4_t get_rotation_matrix_x(float angle);
mat4_t get_rotation_matrix_y(float angle);
mat4_t get_perspective_projection_matrix(float aspect, float fov, float zfar, float znear);

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2);
vec4_t vec4_project(mat4_t m ,vec4_t v);
#endif
