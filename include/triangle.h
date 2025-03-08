#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

typedef struct {
	int a;
	int b;
	int c;

	//texture uv coordinates for each vertices
	tex_t a_uv;
	tex_t b_uv;
	tex_t c_uv;
	
	uint32_t color; //color of face if not using texture
} face_t;


//this structure represent finally when rendering
typedef struct {
	vec2_t   points[3];
	tex_t    texcoords[3];
	uint32_t color;
	float    avg_depth;
} triangle_t;

void draw_triangle(triangle_t t, uint32_t color);
void fill_triangle(triangle_t t, uint32_t color);
void draw_textured_triangle(triangle_t t,uint32_t *texture);

//TODO: fix texture mapping it doesn't work
void draw_texel(int x, int y, uint32_t *texture,vec2_t a, vec2_t b, vec2_t c, tex_t, tex_t, tex_t);
#endif
