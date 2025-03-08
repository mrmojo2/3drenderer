#include "triangle.h"
#include "display.h"
#include "utils.h"
#include "texture.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                         fill_triangle() flat-bottom/flat-top division
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//                                      (x0,y0)
//                                         *
//                                        *  *
//                                       *     *
//                                      *        *
//                                     *           *
//                                    *              *
//                                   *                 *
//                                  *                    *
//                                 *                       *
//                                *                          *
//                               *                             *
//                            (x1,y1)----------------------------(Mx,My)
//                                 *                               *
//                                     *                             *
//                                         *                           *
//                                             *                         *
//                                                 *                       *
//                                                     *                     *
//                                                         *                   *
//                                                             *                 *
//                                                                 *               *
//                                                                     *             *
//                                                                         *           *
//                                                                             *         *
//                                                                                 *       *
//                                                                                     *     *
//                                                                                         *(x2,y2)
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void draw_triangle(triangle_t t, uint32_t color)
{
    draw_line(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, color);
    draw_line(t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, color);
    draw_line(t.points[2].x, t.points[2].y, t.points[0].x, t.points[0].y, color);
}


void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){

/*
	float slope1 =(float) (y1-y0) / (x1-x0);
	float slope2 =(float) (y2-y0) / (x2-x0);

	for(int y = y0; y < y2 ; y++){
		int start_x = ((y-y0) / slope1) + x0;
		int end_x   = ((y-y0) / slope2) + x0;
		draw_line(start_x, y, end_x, y, color);
	}
*/

	float inv_slope_1 = (float) (x1-x0) / (y1-y0);
	float inv_slope_2 = (float) (x2-x0) / (y2-y0);

	float x_start = x0;
	float x_end   = x0;
	for(int y = y0 ; y < y2; y++){
		draw_line(x_start, y, x_end, y, color);
		x_start += inv_slope_1;
		x_end   += inv_slope_2;
	}


}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,uint32_t color){
	
	float inv_slope_1 = (float) (x0-x2) / (y0-y2);
	float inv_slope_2 = (float) (x1-x2) / (y1-y2);

	float x_start = x0;
	float x_end   = x1;
	for(int y = y0;y < y2; y++){
		draw_line(x_start,y,x_end,y,color);
		x_start += inv_slope_1;
		x_end   += inv_slope_2;
	}
}

void fill_triangle(triangle_t t, uint32_t color)
{

	int y0 = t.points[0].y;
	int x0 = t.points[0].x;
	int y1 = t.points[1].y;
	int x1 = t.points[1].x;
	int y2 = t.points[2].y;
	int x2 = t.points[2].x;

	// sorting the vertices by y-cordinate ascending (y0 < y1 < y2)
	if(y0 > y1){
		int_swap(&y0,&y1);
		int_swap(&x0,&x1);
	}
	if(y1>y2){
		int_swap(&y1,&y2);
		int_swap(&x1,&x2);
	}
	if(y0>y1){
		int_swap(&y0,&y1);   	
		int_swap(&x0,&x1);
	}

	//handle special cases (causes divide by 0 inside fill_flat_bottom/top_triangle() )	
	if(y1 == y2){
		//triangle is already a flat bottomed triangle
		fill_flat_bottom_triangle(x0,y0,x1,y1,x2,y2,color);
		return;
	}else if(y0 == y1){
		//triangle is already flat topped triangle
		fill_flat_top_triangle(x0,y0,x1,y1,x2,y2,color);
		return;
	}



	// find point(Mx,My) for flatbottom/flatop partition   
	int My = y1;
	int Mx =(float)(((y1 - y0) * (x2 - x0)) / (float)(y2 - y0)) + x0;



	fill_flat_bottom_triangle(x0,y0,x1,y1,Mx,My,color);
	fill_flat_top_triangle(x1,y1,Mx,My,x2,y2,color);
}

void draw_textured_triangle(triangle_t t,uint32_t *texture){
	int y0 = t.points[0].y;
	int x0 = t.points[0].x;
	int y1 = t.points[1].y;
	int x1 = t.points[1].x;
	int y2 = t.points[2].y;
	int x2 = t.points[2].x;

	
	float v0 = t.texcoords[0].v;
	float u0 = t.texcoords[0].u;
	float v1 = t.texcoords[1].v;
	float u1 = t.texcoords[1].u;
	float v2 = t.texcoords[2].v;
	float u2 = t.texcoords[2].u;

	// sorting the vertices by y-cordinate ascending (y0 < y1 < y2)
	if(y0 > y1){
		int_swap(&y0,&y1);
		int_swap(&x0,&x1);
		float_swap(&v0,&v1);
		float_swap(&u0,&u1);

	}
	if(y1>y2){
		int_swap(&y1,&y2);
		int_swap(&x1,&x2);
		float_swap(&v1,&v2);
		float_swap(&u1,&u2);
	}
	if(y0>y1){
		int_swap(&y0,&y1);   	
		int_swap(&x0,&x1);
		float_swap(&v0,&v1);
		float_swap(&u0,&u1);
	}

	vec2_t a = {x0,y0};
	vec2_t b = {x1,y1};
	vec2_t c = {x2,y2};
	tex_t uva = {u0,v0};
	tex_t uvb = {u1,v1};
	tex_t uvc = {u2,v2};

	
	////////////////////////////////////////////////////////////
	// Render the upper triangle (flat bottom)
	////////////////////////////////////////////////////////////
	float inv_slope1 = 0;
	float inv_slope2 = 0;

	if(y1-y0 != 0) inv_slope1 =(float) (x1-x0) / (y1-y0);
	if(y2-y0 != 0) inv_slope2 =(float) (x2-x0) / (y2-y0);
	
	//scan through the upper triangle
	if(y1-y0 != 0){								//if not lower triangle
		for(int y = y0; y <= y1 ;y++){
			int x_start = x1 + (y-y1)*inv_slope1;
			int x_end   = x0 + (y-y0)*inv_slope2;
			
			if(x_end < x_start) int_swap(&x_start, &x_end);
			
			for(int x = x_start; x < x_end; x++){
				draw_texel(x,y,texture,a,b,c,uva,uvb,uvc);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	//	Render the lower triangle
	////////////////////////////////////////////////////////////////////
	
	if(y2-y1 != 0) inv_slope1 = (float)(x2 - x1)/(y2-y1);
	if(y2-y0 != 0) inv_slope2 = (float)(x2 - x0)/(y2-y0);

	for(int y = y1; y < y2; y++){
		int x_start = x1 + (y - y1)*inv_slope1;
		int x_end = x2 + (y - y2)*inv_slope2;

		if(x_end < x_start ) int_swap(&x_start,&x_end);

		for(int x = x_start; x < x_end; x++){
				draw_texel(x,y,texture,a,b,c,uva,uvb,uvc);
		
		}
	}
}

//TODO:fix this 
void draw_texel(int x, int y, uint32_t *tex,vec2_t a, vec2_t b, vec2_t c, tex_t uv_a, tex_t uv_b, tex_t uv_c){
	
	vec2_t pixel = {x,y};
	vec3_t barycentric_weights = get_barycentric_coords(a,b,c,pixel);

	float alpha = barycentric_weights.x;
	float beta  = barycentric_weights.y;
	float gamma = barycentric_weights.z;


	//finding texel position
	tex_t texel = {
		.u = alpha*(uv_a.u) + beta*(uv_b.u) + gamma * (uv_c.u),
		.v = alpha*uv_a.v + beta*uv_b.v + gamma * uv_c.v
	};

	//fetch color from texture buffer
	int col = texture_width*texel.u - 1;
	int row =texture_height * (1.0 - texel.v);
	int index = texture_width * row + col;
	uint32_t color = 0xff000000;
	if(index >=0 && index < texture_width*texture_height) color = tex[index];

	//draw pixel
	draw_pixel(x,y,color);
	
}
