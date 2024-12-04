#include "triangle.h"
#include "display.h"

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

void swap(int *a, int *b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

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
		swap(&y0,&y1);
		swap(&x0,&x1);
	}
	if(y1>y2){
		swap(&y1,&y2);
		swap(&x1,&x2);
	}
	if(y0>y1){
		swap(&y0,&y1);   	
		swap(&x0,&x1);
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
