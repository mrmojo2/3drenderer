#include "utils.h"
#include "vector.h"

void int_swap(int *a, int *b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
void float_swap(float *a, float *b){
	float tmp = *a;
	*a = *b;
	*b = tmp;
}

vec3_t get_barycentric_coords(vec2_t a,vec2_t b,vec2_t c, vec2_t p){

	//Calculate the area of triangle (actually the area of parallelogram)
	vec2_t AC = vec2_sub(c,a);
	vec2_t AB = vec2_sub(b,a);
	float area_abc = (AC.x*AB.y - AC.y*AB.x);	//magnitude of cross product

	//calculate alpha
	vec2_t PC = vec2_sub(c,p);	
	vec2_t PB = vec2_sub(b,p);
	float area_pcb = (PC.x*PB.y - PC.y*PB.x);
	float alpha = area_pcb/area_abc;


	//calculate betA
	vec2_t PA = vec2_sub(a,p);
	float area_pca =  (PC.x*PA.y - PC.y*PA.x);
	float beta = area_pca/area_abc;
	
	//calculate gamma
	float gamma = 1.0 - alpha - alpha;
		
	vec3_t result = {alpha, beta, gamma};

	return result;
}
