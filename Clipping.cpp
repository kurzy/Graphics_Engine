// Graphics Programming - Week 3 Lab.
// Clipping lines and polygons.
// Jack Kearsley, March 2018.

#include "Header.h"

//======== Defines ================
#define MAX_VERTICES 20

// Clips a line (p1 to p2) using a rectangle shape specified as 4 maximum points (left, right, top, bottom).
// Uses the Liang-Barsky method.
void ClipLine(BYTE * pFrame, int * zbuff, int left, int right, int top, int bottom, POINT3D p1, POINT3D p2, COLOR c) {
	double u1 = 0.0, u2 = 1.0;
	double dy = 0;
	double dx = p2.x - p1.x;
	if (clip_test(-dx, p1.x - left, &u1, &u2)) {
		if (clip_test(dx, right-p1.x, &u1, &u2)) {
			dy = p2.y - p1.y;
			if (clip_test(-dy, p1.y-bottom, &u1, &u2)) {
				if (clip_test(dy, top-p1.y, &u1, &u2)) {
					if (u2 < 1.0) {
						p2.x = p1.x + u2 * dx;
						p2.y = p1.y + u2 * dy;
					}
					if (u1 > 0.0) {
						p1.x += u1 * dx;
						p1.y += u1 * dy;
					}
					DDA_DrawLine(pFrame, zbuff, p1.x, p1.y, p2.x, p2.y, c.r, c.g, c.b);
				}
			}
		}
	}	
}


bool clip_test(double p, double q, double * u1, double * u2) {
	double r = q / p;
	if (p < 0.0) {
		if (r > *u2) return false;
		else if (r > *u1) *u1 = r;
	}
	else if (p > 0.0) {
		if (r < *u1) return false;
		else if (r < *u2) *u2 = r;
	}
	// p == 0 so line is parallel to clip boundary
	else if (q < 0.0) return false;
	
	return true;
}



// Clip a polygon using the Sutherland-Hodgeman method. 
void ClipPolygon(BYTE * pFrame, int * zbuff, int left, int right, int top, int bottom, const unsigned int n_vertices, POINT3D * vertex, COLOR * color) {	
	// Make a copy of the vertices and colors so the originals are not overwritten.
	POINT3D a_poly[MAX_VERTICES];
	COLOR a_colr[MAX_VERTICES];
	for (int i = 0; i < n_vertices; i++) {
		a_poly[i] = vertex[i];
		a_colr[i] = color[i];
	}

	// Each line_bound() call clips the polygon on a separate side (LEFT, RIGHT, TOP, BOTTOM).
	// The output of each line_bound() is then passed to the next call (a_poly).
	unsigned int in_vertices = n_vertices;
	line_bound(LEFT, left, &in_vertices, a_poly, a_colr);
	line_bound(RIGHT, right, &in_vertices, a_poly, a_colr);
	line_bound(TOP, top, &in_vertices, a_poly, a_colr);
	line_bound(BOTTOM, bottom, &in_vertices, a_poly, a_colr);
	if (in_vertices <= 0) return;

	// Draw the clipped polygon.
	Polygon(pFrame, zbuff, in_vertices, a_poly, a_colr);
}


// Clip the polygon against a specfied line value (line_val) for one of (LEFT, RIGHT, TOP, BOTTOM).
void line_bound(enum side clip_line, int line_val, unsigned int * n_vertices, POINT3D * vertex, COLOR * color) {
	unsigned int outv_count = 0;
	POINT3D out_vertex[MAX_VERTICES];
	COLOR out_color[MAX_VERTICES];
	unsigned int num_vertices = *n_vertices;

	for (int v = 0; v < num_vertices; v++) {
		int point_a = v;
		int point_b = (v + 1) % num_vertices;
		bool a_inside = false;
		bool b_inside = false;
		if (clip_line == LEFT) {
			a_inside = (vertex[point_a].x >= line_val) ? true : false;
			b_inside = (vertex[point_b].x >= line_val) ? true : false;
		}
		else if (clip_line == RIGHT) {
			a_inside = (vertex[point_a].x <= line_val) ? true : false;
			b_inside = (vertex[point_b].x <= line_val) ? true : false;
		}
		else if (clip_line == TOP) {
			a_inside = (vertex[point_a].y <= line_val) ? true : false;
			b_inside = (vertex[point_b].y <= line_val) ? true : false;
		}
		else if (clip_line == BOTTOM) {
			a_inside = (vertex[point_a].y >= line_val) ? true : false;
			b_inside = (vertex[point_b].y >= line_val) ? true : false;
		}
		// The scenarios that can occur when clipping the line.
		if (a_inside && b_inside) {
			out_vertex[outv_count] = vertex[point_b];
			out_color[outv_count] = color[point_b];
			outv_count++;
		}
		else if (a_inside && !b_inside) {
			POINT3D intersect;
			COLOR intersect_color;
			get_line_intersect(vertex[point_a], vertex[point_b], color[point_a], color[point_b], clip_line, line_val, &intersect, &intersect_color);
			out_vertex[outv_count] = intersect;
			out_color[outv_count] = intersect_color;
			outv_count++;
		}
		else if (!a_inside && b_inside) {
			POINT3D intersect;
			COLOR intersect_color;
			get_line_intersect(vertex[point_a], vertex[point_b], color[point_a], color[point_b], clip_line, line_val, &intersect, &intersect_color);
			out_vertex[outv_count] = intersect;
			out_color[outv_count] = intersect_color;
			outv_count++;
			out_vertex[outv_count] = vertex[point_b];
			out_color[outv_count] = color[point_b];
			outv_count++;
		}
		// Else if (both points are not inside) don't add any of the points.

	}

	// Update the number of vertices for the outputted polygon.
	*n_vertices = outv_count;
	// Update the vertex array with the new vertices.
	for (int i = 0; i < outv_count; i++) {
		vertex[i] = out_vertex[i];
		color[i] = out_color[i];
	}
}


// Calculates the point at which a line (p1-p2) intersects with a bounding line (left, right, top, bottom) of value (line_val).
void get_line_intersect(POINT3D p1, POINT3D p2, COLOR c1, COLOR c2, enum side clip_line, int line_val, POINT3D * intersect, COLOR * intersect_color) {
	if (clip_line == LEFT || clip_line == RIGHT) {
		intersect->x = line_val;
		double dy = (double)(p2.y - p1.y) / (p2.x - p1.x);
		double ypoint = p1.y + (dy * (line_val - p1.x));
		
		double dz = (double)(p2.z - p1.z) / (p2.x - p1.x);
		double zpoint = p1.z + (dz * (line_val - p1.x));

		intersect->y = (int)ypoint;
		intersect->z = (int)zpoint;

		double mR = (double)(c2.r - c1.r) / (p2.x - p1.x);
		double mG = (double)(c2.g - c1.g) / (p2.x - p1.x);
		double mB = (double)(c2.b - c1.b) / (p2.x - p1.x);

		unsigned char r = c1.r + (mR * (line_val - p1.x));
		unsigned char g = c1.g + (mG * (line_val - p1.x));
		unsigned char b = c1.b + (mB * (line_val - p1.x));

		intersect_color->r = r;
		intersect_color->g = g;
		intersect_color->b = b;

	}
	else if (clip_line == TOP || clip_line == BOTTOM) {
		intersect->y = line_val;
		double dx = (double)(p2.x - p1.x) / (p2.y - p1.y);
		double xpoint = p1.x + (dx * (line_val - p1.y));

		double dz = (double)(p2.z - p1.z) / (p2.y - p1.y);
		double zpoint = p1.z + (dz * (line_val - p1.y));

		intersect->x = (int)xpoint;
		intersect->z = (int)zpoint;

		double mR = (double)(c2.r - c1.r) / (p2.y - p1.y);
		double mG = (double)(c2.g - c1.g) / (p2.y - p1.y);
		double mB = (double)(c2.b - c1.b) / (p2.y - p1.y);

		double r = c1.r + (mR * (line_val - p1.y));
		double g = c1.g + (mG * (line_val - p1.y));
		double b = c1.b + (mB * (line_val - p1.y));

		intersect_color->r = r;
		intersect_color->g = g;
		intersect_color->b = b;

	}
}


