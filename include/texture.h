#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
	float u;
	float v;
} tex_t;

extern unsigned texture_width;
extern unsigned texture_height;

extern uint32_t *texture;

void load_png_texture(const char *);

#endif
