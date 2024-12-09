#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4_t;
///////////////////////////////////////////////////////////////////////////////////////
// 2D Vector functions
// ///////////////////////////////////////////////////////////////////////////////////
float  vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t, vec2_t);
vec2_t vec2_sub(vec2_t, vec2_t);
vec2_t vec2_mul(vec2_t, float);
vec2_t vec2_div(vec2_t, float);
float  vec2_dot(vec2_t, vec2_t);

//////////////////////////////////////////////////////////////////////////////////////
// 3d Vector functions
// ///////////////////////////////////////////////////////////////////////////////////
float  vec3_length(vec3_t v);
vec3_t vec3_add(vec3_t, vec3_t);
vec3_t vec3_sub(vec3_t, vec3_t);
vec3_t vec3_mul(vec3_t, float);
vec3_t vec3_div(vec3_t, float);
vec3_t vec3_cross(vec3_t, vec3_t);
float  vec3_dot(vec3_t, vec3_t);
vec3_t vec3_get_unit(vec3_t);

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

vec3_t get_vec3(vec4_t v);

/////////////////////////////////////////////////////////////////////////////////////
// 4D vector function
/////////////////////////////////////////////////////////////////////////////////////
vec4_t get_vec4(vec3_t);

#endif
