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

mat4_t mat4_null(void){
	mat4_t result = {{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	}};

	return result;
}

mat4_t get_scale_matrix(float sx, float sy, float sz){
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

mat4_t get_translation_matrix(float tx, float ty, float tz){
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

mat4_t get_rotation_matrix_z(float angle){
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


mat4_t get_rotation_matrix_x(float angle){
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

mat4_t get_rotation_matrix_y(float angle){
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

mat4_t get_perspective_projection_matrix(float aspect, float fov, float zfar, float znear){

	// |  af  0  0  0    |
	// |  0   f  c  0    |
	// |  0   0  l -l*zn |
	// |  0   0  1  0    |

	float lamda = zfar/(zfar-znear);
	float f = 1/tan(fov/2);

	mat4_t m  = mat4_null();
	
	m.m[0][0] = aspect*f;
	m.m[1][1] = f;
	m.m[2][2] = lamda;
	m.m[2][3] = -lamda*znear;
	m.m[3][2] = 1.0;

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

mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2){
	mat4_t result = mat4_null();

	for(int r=0;r<4;r++){
		for(int c=0;c<4;c++){
			for(int i=0;i<4;i++){
				result.m[r][c] += (m1.m[r][i]*m2.m[i][c]); 
			}
		}
	}	

	return result;
}

vec4_t vec4_project(mat4_t m, vec4_t v){
	vec4_t result = mat4_mul_vec4(m,v);

	//perform perspective divide
	if(result.w != 0.0 ){
		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;
	}

	return result;
}
