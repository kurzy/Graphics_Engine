// 3802ICT Interactive Graphics Programming and Visualisation.
// Handles mouse input.
// Jack Kearsley, March 2018.

#include "Header.h"
#include <limits.h>

// Returns true if the user has clicked inside one of the 'active_objects'.
// If the user has clicked an active object, the object's status is set to false, and it will disappear from the screen.
// Draws a horizontal line to infinity in one direction, and counts the number of times the line intersects with the polygon.
// If the number of intersects is odd, the point is inside the polygon, and if even, the point is not inside the polygon.

bool process_click(_Object * act_obj, bool * status, int x, int y) {
	// For each active object.
	for (int o = 0; o < NUM_MY_OBJECTS; o++) {
		if (!status[o]) continue;

		// Check that the mouse is within the minimum and maximum x and y values.
		int min_x, min_y;
		min_x = min_y = INT_MAX;
		int max_x, max_y;
		max_x = max_y = INT_MIN;
		for (int v = 0; v < act_obj[o].num_points; v++) {
			POINT3D pt = ProjectTo2D(act_obj[o].ObjectPoints[v]);
			if (pt.x < min_x) min_x = pt.x;
			if (pt.x > max_x) max_x = pt.x;
			if (pt.y < min_y) min_y = pt.y;
			if (pt.y > max_y) max_y = pt.y;
		}
		// If the click point is not within these, we can exit from the function now.
		if (x < min_x || x > max_x || y < min_y || y > max_y) continue;

		// For each polygon in the object.
		for (int p = 0; p < act_obj[o].num_polys; p++) {
			int nvert = act_obj[o].ObjectPolys[p].num_vertices;
			int intersect_count = 0;
			// For each vertex in the polygon.
			for (int v = 0; v < nvert; v++) {
				int vx_id_a = act_obj[o].ObjectPolys[p].vertex[v];
				int vx_id_b = act_obj[o].ObjectPolys[p].vertex[(v+1) % nvert];
				POINT3D apt = ProjectTo2D(act_obj[o].ObjectPoints[vx_id_a]);
				POINT3D bpt = ProjectTo2D(act_obj[o].ObjectPoints[vx_id_b]);

				double dy = (double)(bpt.y - apt.y) / (bpt.x - apt.x);

				double x_point = (double)(y - apt.y) / dy + apt.x;
				if (x_point >= x) intersect_count++;

			}//Vertices
			if (intersect_count % 2 == 1) {
				//printf("Point was inside polygon, setting slot %d to false\n", o);
				status[o] = false;
				return true;
			}

		}//Polygons

	}//Active_objects

	return false;
}


// Projects a 3D point to a 2D screen, taking into account its z-value.
POINT3D ProjectTo2D(POINT3D a) {
	int sc = 500;
	double new_x = ((double)((a.x - FRAME_WIDE / 2)*sc) / (a.z + sc)) + FRAME_WIDE / 2;
	double new_y = ((double)((a.y - FRAME_HIGH / 2)*sc) / (a.z + sc)) + FRAME_HIGH / 2;
	return {(int)new_x, (int)new_y, a.z};
}



///////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////
