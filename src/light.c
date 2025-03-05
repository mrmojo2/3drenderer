#include "light.h"

light_t light = {
	.direction = {
		.x = 1,
		.y = -1,
		.z = 0
	}
};

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor){
	uint32_t a = (original_color & 0xff000000);
	uint32_t r = (original_color & 0x00ff0000)*percentage_factor;
	uint32_t g = (original_color & 0x0000ff00)*percentage_factor;
	uint32_t b = (original_color & 0x000000ff)*percentage_factor;

	uint32_t new_color = a | (r & 0x00ff0000) | (g & 0x0000ff00) | (b & 0x000000ff);
	return new_color;
}
