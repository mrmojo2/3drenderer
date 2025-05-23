#include "triangle.h"
#include "display.h"
#include "utils.h"
#include "texture.h"
#include "options.h"
#include "light.h"
#include "mesh.h"
#include "array.h"

void draw_triangle(triangle_t t, uint32_t color)
{
    draw_line(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, color);
    draw_line(t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, color);
    draw_line(t.points[2].x, t.points[2].y, t.points[0].x, t.points[0].y, color);
}



vec3_t get_vertex_avgn(int vi){
	vec3_t result = {0,0,0};
	
	face_t** neighbors = (face_t **) mesh.vertex_neighbor_list[vi-1];
	int s = array_length(neighbors);
	for(int i=0; i<s; i++){
		vec3_t n = vec3_get_unit(neighbors[i]->normal);
		result = vec3_add(result,n);		
	}
	result =  vec3_div(result,s);
	return vec3_get_unit(result);
}

void fill_triangle(triangle_t *t,uint32_t color){
	int x0 = (t->points[0].x);
	int y0 = (t->points[0].y);
	int z0 = (t->points[0].z);
	int w0 = (t->points[0].w);
	int a = t->face->a;
	
	int y1 = (t->points[1].y);
	int x1 = (t->points[1].x);
	int z1 = (t->points[1].z);
	int w1 = (t->points[1].w);
	int b = t->face->b;
	
	int y2 = (t->points[2].y);
	int x2 = (t->points[2].x);
	int z2 = (t->points[2].z);
	int w2 = (t->points[2].w);
	int c = t->face->c;
	
	// sorting the triangle t  by y-cordinate ascending (y0 < y1 < y2)
	if(y0 > y1){
		int_swap(&y0,&y1);
		int_swap(&x0,&x1);
		int_swap(&w0,&w1);
		int_swap(&z0,&z1);
		//int_swap(&a,&b);

	}
	if(y1 > y2){
		int_swap(&y1,&y2);
		int_swap(&x1,&x2);
		int_swap(&w1,&w2);
		int_swap(&z1,&z2);
		//int_swap(&b,&c);
	}
	if(y0 > y1){
		int_swap(&y0,&y1);   	
		int_swap(&x0,&x1);
		int_swap(&w0,&w1);
		int_swap(&z0,&z1);
		//int_swap(&a,&b);
	}


	
	////////////////////////////////////////////////////////////
	// Render the upper triangle (flat bottom)
	////////////////////////////////////////////////////////////
	float inv_slope1 = 0;
	float inv_slope2 = 0;


	if(y1 != y0) inv_slope1 =(float) (x1-x0) / (y1-y0);
	if(y2 != y0) inv_slope2 =(float) (x2-x0) / (y2-y0);
	
	/*vec3_t a = {x0,y0,z0};
	vec3_t b = {x1,y1,z1};
	vec3_t c = {x2,y2,z2};

	vec3_t normal_a = vec3_get_unit(vec3_cross(vec3_sub(b,a),vec3_sub(c,a))); //AB X AC
	vec3_t normal_b = vec3_get_unit(vec3_cross(vec3_sub(c,b),vec3_sub(a,b))); //BC X BA
	vec3_t normal_c = vec3_get_unit(vec3_cross(vec3_sub(a,c),vec3_sub(b,c))); //CA X CB

	float dot_a = vec3_dot(normal_a,light_normal); 
	float dot_b = vec3_dot(normal_b,light_normal); 
	float dot_c = vec3_dot(normal_c,light_normal); */

	vec3_t light_normal = vec3_get_unit(light.direction);
	
	vec3_t avgn_a = get_vertex_avgn(a);
	vec3_t avgn_b = get_vertex_avgn(b);
	vec3_t avgn_c = get_vertex_avgn(c);

	float dot_a = vec3_dot(avgn_a,light_normal); 
	float dot_b = vec3_dot(avgn_b,light_normal); 
	float dot_c = vec3_dot(avgn_c,light_normal);

	
	//scan through the upper triangle
	if(y1 != y0){								//if not lower triangle
		for(int y = y0; y <= y1 ;y++){
			int x_start = x1 + (y-y1)*inv_slope1;
			int x_end   = x0 + (y-y0)*inv_slope2;
			if(!GOUROUD_SHADING){	
				draw_line(x_start,y,x_end,y,color);
			}else{	
				
				if(x_end < x_start) int_swap(&x_start, &x_end);
					
				for(int x = x_start; x < x_end; x++){
					//calculate barycentric coords of current point
					vec2_t p = {x,y};
					vec2_t A = {t->points[0].x,t->points[0].y};
					vec2_t B = {t->points[1].x,t->points[1].y};
					vec2_t C = {t->points[2].x,t->points[2].y};

					vec3_t weights = get_barycentric_coords(A,B,C,p);
					float alpha = weights.x;
					float beta = weights.y;
					float gamma = weights.z;

					//interpolate the dot product value at point p
					float dot_p = alpha * dot_a + beta * dot_b + gamma * dot_c;
					uint32_t c = light_apply_intensity(color, 1-(dot_p + 1)/2);
					draw_pixel(x,y,c);


					
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	//	Render the lower triangle
	////////////////////////////////////////////////////////////////////
	
	if(y2 != y1) inv_slope1 = (float)(x2 - x1)/(y2-y1);
	if(y2 != y0) inv_slope2 = (float)(x2 - x0)/(y2-y0);

	for(int y = y1; y < y2; y++){
		int x_start = x1 + (y - y1)*inv_slope1;
		int x_end = x0 + (y - y0)*inv_slope2;

		if(!GOUROUD_SHADING){	
			draw_line(x_start,y,x_end,y,color);
		}else{	
			
			if(x_end < x_start) int_swap(&x_start, &x_end);
				
			for(int x = x_start; x < x_end; x++){
				//calculate barycentric coords of current point
				vec2_t p = {x,y};
				vec2_t A = {t->points[0].x,t->points[0].y};
				vec2_t B = {t->points[1].x,t->points[1].y};
				vec2_t C = {t->points[2].x,t->points[2].y};

				vec3_t weights = get_barycentric_coords(A,B,C,p);
				float alpha = weights.x;
				float beta = weights.y;
				float gamma = weights.z;

				//interpolate the dot product value at point p
				float dot_p = alpha * dot_a + beta * dot_b + gamma * dot_c;

				uint32_t c = light_apply_intensity(color, 1-(dot_p + 1)/2);
				draw_pixel(x,y,c);
			}
		}
	}
	
}



//TODO: THESE FLOATING POINTS ARE VERY BAD...i think
void draw_textured_triangle(triangle_t *t,uint32_t *texture){
	float *x0 = &(t->points[0].x);
	float *y0 = &(t->points[0].y);
	float *z0 = &(t->points[0].z);
	float *w0 = &(t->points[0].w);
	
	float *y1 = &(t->points[1].y);
	float *x1 = &(t->points[1].x);
	float *z1 = &(t->points[1].z);
	float *w1 = &(t->points[1].w);
	
	float *y2 = &(t->points[2].y);
	float *x2 = &(t->points[2].x);
	float *z2 = &(t->points[2].z);
	float *w2 = &(t->points[2].w);

	
	float *v0 = &(t->texcoords[0].v);
	float *u0 = &(t->texcoords[0].u);
	float *v1 = &(t->texcoords[1].v);
	float *u1 = &(t->texcoords[1].u);
	float *v2 = &(t->texcoords[2].v);
	float *u2 = &(t->texcoords[2].u);

	
	// sorting the triangle t  by y-cordinate ascending (y0 < y1 < y2)
	if(*y0 > *y1){
		float_swap(y0,y1);
		float_swap(x0,x1);
		float_swap(v0,v1);
		float_swap(u0,u1);
		float_swap(w0,w1);
		float_swap(z0,z1);

	}
	if(*y1 > *y2){
		float_swap(y1,y2);
		float_swap(x1,x2);
		float_swap(v1,v2);
		float_swap(u1,u2);
		float_swap(w1,w2);
		float_swap(z1,z2);
	}
	if(*y0 > *y1){
		float_swap(y0,y1);   	
		float_swap(x0,x1);
		float_swap(v0,v1);
		float_swap(u0,u1);
		float_swap(w0,w1);
		float_swap(z0,z1);
	}


	
	////////////////////////////////////////////////////////////
	// Render the upper triangle (flat bottom)
	////////////////////////////////////////////////////////////
	float inv_slope1 = 0;
	float inv_slope2 = 0;

	float inv_w0 = 1.0 / *w0;
	float inv_w1 = 1.0 / *w1;
	float inv_w2 = 1.0 / *w2;

	if(*y1-*y0 > 1e-4f) inv_slope1 =(float) (*x1-*x0) / (*y1-*y0);
	if(*y2-*y0 > 1e-4f) inv_slope2 =(float) (*x2-*x0) / (*y2-*y0);
	
	//scan through the upper triangle
	if(*y1-*y0 > 1e-4f){								//if not lower triangle
		for(float y = *y0; y <= *y1 ;y++){
			int x_start = *x1 + (y-*y1)*inv_slope1;
			int x_end   = *x0 + (y-*y0)*inv_slope2;
			
			if(x_end < x_start) int_swap(&x_start, &x_end);
			for(int x = x_start; x < x_end; x++){
				draw_texel(x,y,texture,t,inv_w0,inv_w1,inv_w2);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	//	Render the lower triangle
	////////////////////////////////////////////////////////////////////
	
	if(*y2-*y1 > 1e-4f) inv_slope1 = (float)(*x2 - *x1)/(*y2-*y1);
	if(*y2-*y0 > 1e-4f) inv_slope2 = (float)(*x2 - *x0)/(*y2-*y0);

	for(float y = *y1; y < *y2; y++){
		int x_start = *x1 + (y - *y1)*inv_slope1;
		int x_end = *x2 + (y - *y2)*inv_slope2;

		if(x_end < x_start ) int_swap(&x_start,&x_end);

		for(int x = x_start; x < x_end; x++){
				draw_texel(x,y,texture,t,inv_w0,inv_w1,inv_w2);
		
		}
	}
	
}

//TODO:fix this 
void draw_texel(int x, int y, uint32_t *tex,triangle_t *t,float inv_wa, float inv_wb, float inv_wc){	
	vec2_t p = {x,y};
	vec2_t a = {t->points[0].x,t->points[0].y};
	vec2_t b = {t->points[1].x,t->points[1].y};
	vec2_t c = {t->points[2].x,t->points[2].y};
	
	vec3_t barycentric_weights = get_barycentric_coords(a,b,c,p);

	float alpha = barycentric_weights.x;
	float beta  = barycentric_weights.y;
	float gamma = barycentric_weights.z;
	

	//interpolated values of u,v and 1/w for point P(x,y)
	float interpolated_u;
	float interpolated_v;
	float interpolated_invw;
	
	//interpolate the values of u/w and v/w
	interpolated_u = alpha * (t->texcoords[0].u * inv_wa) + beta * (t->texcoords[1].u * inv_wb) + gamma * (t->texcoords[2].u * inv_wc);
	interpolated_v = alpha * (t->texcoords[0].v * inv_wa) + beta * (t->texcoords[1].v * inv_wb) + gamma * (t->texcoords[2].v * inv_wc);
	interpolated_invw = alpha * (inv_wa) + beta * (inv_wb) +gamma * (inv_wc);

	//undo the effect of 1/w on interpolated u and v
	interpolated_u /= interpolated_invw;
	interpolated_v /= interpolated_invw;



	//fetch color from texture buffer
	int col = (texture_width - 1) * interpolated_u;
	int row = (texture_height - 1) * (1.0 - interpolated_v);
	int index = texture_width * row + col;
	uint32_t color = 0xff000000;
	if(index >=0 && index < texture_width*texture_height) color = tex[index];

	//draw pixel by applying perspective correct interpolation
	draw_pixel(x,y,color);
	
}
