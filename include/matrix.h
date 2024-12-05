#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
	float m[4][4];
} mat4_t;


mat4_t mat4_identity(void);

mat4_t mat4_get_scale_matrix(float sx, float sy, float sz);
mat4_t mat4_get_translation_matrix(float tx, float ty, float tz);
mat4_t mat4_get_rotation_matrix_z(float angle);
mat4_t mat4_get_rotation_matrix_x(float angle);
mat4_t mat4_get_rotation_matrix_y(float angle);


vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

#endif
