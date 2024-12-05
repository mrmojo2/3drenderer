#include "matrix.h"

#include <math.h>

mat4_t mat4_identity(void){
	mat4_t result = {{
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	}};

	return result;
}

mat4_t mat4_get_scale_matrix(float sx, float sy, float sz){
	// | sx  0   0  0 |
	// | 0  sy   0  0 |
	// | 0   0  sz  0 |
	// | 0   0   0  1 |


	mat4_t m = mat4_identity();
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;

	return m;
}

mat4_t mat4_get_translation_matrix(float tx, float ty, float tz){
	// | 1  0  0  tx |
	// | 0  1  0  ty |
	// | 0  0  1  tz |
	// | 0  0  0  1  |


	mat4_t m = mat4_identity();
	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;

	return m;
}

mat4_t mat4_get_rotation_matrix_z(float angle){
	float c = cos(angle);
	float s = sin(angle);
	// | c  -s  0  0 |
	// | s   c  0  0 |
	// | 0   0  1  0 |
	// | 0   0  0  1 |
	
	mat4_t m = mat4_identity();
	m.m[0][0] =  c;
	m.m[0][1] = -s;
	m.m[1][0] =  s;
	m.m[1][1] =  c;

	return m;
}


mat4_t mat4_get_rotation_matrix_x(float angle){
	float c = cos(angle);
	float s = sin(angle);
	// | 1   0   0  0 |
	// | 0   c  -s  0 |
	// | 0   s   c  0 |
	// | 0   0   0  1 |
	
	mat4_t m = mat4_identity();
	m.m[1][1] =  c;
	m.m[1][2] = -s;
	m.m[2][1] =  s;
	m.m[2][2] =  c;

	return m;
}

mat4_t mat4_get_rotation_matrix_y(float angle){
	float c = cos(angle);
	float s = sin(angle);
	// |  c   0  s  0 |
	// | -s   1  c  0 |
	// |  0   0  1  0 |
	// |  0   0  0  1 |
	
	mat4_t m = mat4_identity();
	m.m[0][0] =  c;
	m.m[0][2] =  s;			//the signs of sin are changed to account for our coordinte system
	m.m[2][0] = -s;
	m.m[2][2] =  c;

	return m;
}

vec4_t mat4_mul_vec4(mat4_t m , vec4_t v){
	vec4_t result;

	result.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3];
	result.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3];
	result.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3];
	result.w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3];
	
	return result;
}
