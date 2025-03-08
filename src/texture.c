#include "texture.h"
#include "lodepng.h"
#include "display.h"
#include "utils.h"
#include "triangle.h"

#include <stdio.h>
#include <stdlib.h>

unsigned texture_width;
unsigned texture_height;
uint32_t *texture = NULL;

void load_png_texture(const char *filename){
	unsigned char *image = 	NULL;
	unsigned error = lodepng_decode32_file(&image,&texture_width,&texture_height,filename);
	if(error){
		printf("PNG Decoder error %u: %s\n",error,lodepng_error_text(error));
		return;
	}

	texture = (uint32_t*)malloc(texture_width*texture_height*sizeof(uint32_t));
	if(!texture){
		free(image);
		printf("Couldn't allocate memory for texture \n");
		return;
	}

	for(size_t i=0; i<texture_width * texture_height;i++){
		uint8_t r = image[i * 4];
		uint8_t g = image[i * 4 + 1];
		uint8_t b = image[i * 4 + 2];
		uint8_t a = image[i * 4 + 3];

		texture[i] = (a<<24) | (r << 16) | (g << 8) | b;
	}

	free(image);
}
