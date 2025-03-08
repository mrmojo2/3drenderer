#ifndef UTILS_H
#define UTILS_H

#include "vector.h"

vec3_t get_barycentric_coords(vec2_t a,vec2_t b,vec2_t c, vec2_t p);
void int_swap(int *a, int *b);
void float_swap(float *a, float *b);


#endif
