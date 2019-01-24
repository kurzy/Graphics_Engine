// Graphics Programming - Week 3 Lab.
// Drawing n-sided polygons using triangle decomposition.
// Jack Kearsley, March 2018.

#include "Header.h"
#include <vector>
#include <math.h>


int SameSide(POINT3D a, POINT3D b, POINT3D l1, POINT3D l2) 
{ //--- l1 and l2 are the ends of the line
  //--- returns 1 if a & b are on the same side of line
	int apt = (a.x - l1.x) * (l2.y - l1.y) - (l2.x - l1.x) * (a.y - l1.y);
	int bpt = (b.x - l1.x) * (l2.y - l1.y) - (l2.x - l1.x) * (b.y - l1.y);
	return ((apt * bpt) > 0);
}

int inside(POINT3D p, POINT3D a, POINT3D b, POINT3D c)
{
	return SameSide(p, a, b, c)
		&& SameSide(p, b, a, c)
		&& SameSide(p, c, a, b);
}

// Modulo function that can deal with negative numbers.
int mod(int a, int b)
{
	int r = a % b;
	return r < 0 ? r + b : r;
}





void Polygon(BYTE * pFrame, int * zbuff, unsigned int n_vertices, POINT3D * vertex, COLOR * color) {
	if (n_vertices < 3) {
		printf("Error: for Polygon() function, n_vertices < 3.\n");
		exit(1);
	}

	// active_edges stores edges that have been drawn already.
	const unsigned int max_edges = 10;
	bool active_edge[max_edges];
	for (int i = 0; i < max_edges; i++) active_edge[i] = false;


	// Main loop for each vertex.
	for (int v = 0; v < n_vertices-2; v++) {

		// Find left-most (smallest x) vertex.
		POINT3D left = { 0,0,0 };
		COLOR left_color = { 0,0,0 };
		int min = INT_MAX;
		int idx = 0;
		for (int i = 0; i < n_vertices; i++) {
			if (vertex[i].x < min && !active_edge[i]) {
				left = vertex[i];
				left_color = color[i];
				min = vertex[i].x;
				idx = i;
			}
		}
		active_edge[idx] = true;

		//POINT3D left = vertex[v];
		//COLOR left_color = color[v];
		//int idx = v;
		//active_edge[idx] = true;
		
		// Get the adjacent vertices.
		int adj_a = 0, adj_b = 0;
		bool adj_a_found = false, adj_b_found = false;

		// Try find the next adjacent vertex by increasing the indices from the left point.
		for (int i = idx + 1; i < (idx + n_vertices); i++) {
			if (!active_edge[i % n_vertices]) {
				adj_a = i % n_vertices;
				adj_a_found = true;
				break;
			}
		}
		// Find the next adjacent vertex by decreasing the indices from the left point.
		for (int i = 1; i < (idx + n_vertices); i++) {
			int num = mod(idx-i, n_vertices);
			if (!active_edge[num]) {
				adj_b = num;
				adj_b_found = true;
				break;
			}
		}

		
		// If there's no more adjacent vertices to draw, we have finished drawing the polygon, and can return from the function.
		if (!adj_a_found && !adj_b_found) return;
		active_edge[adj_a] = true;
		active_edge[adj_b] = true;

		// Check to see that no other vertex is inside the triangle.
		// If there are points inside, store them so we can try to draw a new and correct triangle.
		bool point_inside = false;
		POINT3D in_point[max_edges - 3];
		COLOR in_point_col[max_edges - 3];
		int in_point_count = 0;
		for (int i = 0; i < n_vertices; i++) {
			if (active_edge[i]) continue;
			if (inside(vertex[i], left, vertex[adj_a], vertex[adj_b])) {
				point_inside = true;
				in_point[in_point_count] = vertex[i];
				in_point_col[in_point_count] = color[i];
				in_point_count++;
			}
		}




		// If no points were inside the triangle, we can paint it to the screen.
		if (!point_inside) {
			POINT3D drawTri[3] = { vertex[idx], vertex[adj_a], vertex[adj_b] };
			COLOR drawTriCol[3] = { color[idx], color[adj_a], color[adj_b] };
			Triangle(pFrame, zbuff, drawTri, drawTriCol);
		}
		// If there are points inside, then form a new triangle with the leftmost inside point.
		else {
			
			//active_edge[idx] = false;
			
			// Get the leftmost point inside the triangle.
			POINT3D leftmost_inside = { 0,0,0 };
			COLOR leftmost_inside_col = { 0,0,0 };
			int leftmost_inside_ind = 0;
			int min = INT_MAX;
			for (int i = 0; i < in_point_count; i++) {
				if (in_point[i].x < min) {
					min = in_point[i].x;
					leftmost_inside_ind = i;
					leftmost_inside = in_point[i];
					leftmost_inside_col = in_point_col[i];
				}
			}

			// Get the leftmost, outside point of the two adjacent points (adj_a and adj_b).
			POINT3D leftmost_outside = { 0,0,0 };
			COLOR leftmost_outside_col = { 0,0,0 };
			int leftmost_outside_ind = 0;
			if (vertex[adj_a].x < vertex[adj_b].x) {
				leftmost_outside = vertex[adj_a];
				leftmost_outside_col = color[adj_a];
				leftmost_outside_ind = adj_a;
			}
			else {
				leftmost_outside = vertex[adj_b];
				leftmost_outside_col = color[adj_b];
				leftmost_outside_ind = adj_b;
			}

			// Draw two triangles.
			POINT3D drawTri[3] = { vertex[idx], leftmost_inside, vertex[adj_a] };
			COLOR drawTriCol[3] = { color[idx], leftmost_inside_col, color[adj_a] };
			Triangle(pFrame, zbuff, drawTri, drawTriCol);

			POINT3D second_tri[3] = { vertex[idx], leftmost_outside, leftmost_inside };
			COLOR second_tri_col[3] = { color[idx], leftmost_outside_col, leftmost_inside_col };
			Triangle(pFrame, zbuff, second_tri, second_tri_col);
		}

		// Reset the adjacnet vertices for the next iteration.
		active_edge[adj_a] = false;
		active_edge[adj_b] = false;
	}
	
}









