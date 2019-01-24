// 3802ICT Interactive Graphics Programming and Visualisation.
// Drawing 3D polygons.
// Jack Kearsley, March 2018.

#include "Header.h"
#include <string.h>

// Draws a 3D object passed to the function as an _Object struct.
// The 3D object uses a VJS representation.

void Draw_3D_Object(BYTE * pFrame, int * zbuff, _Object * obj) {
	int sc = 500;

	// If no points are inside the frame, don't bother drawing the 3D object.
	bool drawable = false;
	for (int i = 0; i < obj->num_points; i++) {
		POINT3D pt = obj->ObjectPoints[i];
		pt.x = ((double)((pt.x - FRAME_WIDE / 2)*sc) / (pt.z + sc)) + FRAME_WIDE / 2;
		pt.y = ((double)((pt.y - FRAME_HIGH / 2)*sc) / (pt.z + sc)) + FRAME_HIGH / 2;
		if ((pt.x >= 0 && pt.x <= FRAME_WIDE) || (pt.y >= 0 && pt.y <= FRAME_HIGH)) {
			drawable = true;
			break;
		}
	}
	if (!drawable) return;
	
	int number_of_verts = obj->num_points;

	// For each polygon in the 3D object.
	for (int p = 0; p < obj->num_polys; p++) {
		POINT3D poly_points[POINTS_PER_POLYGON];
		COLOR poly_colors[POINTS_PER_POLYGON];
		int num_v = obj->ObjectPolys[p].num_vertices;

		// For each vertex in the polygon.
		for (int v = 0; v < num_v; v++) {
			// Vertex ID stored in vertex[].
			int vx_id = obj->ObjectPolys[p].vertex[v];
			int x = obj->ObjectPoints[vx_id].x;
			int y = obj->ObjectPoints[vx_id].y;
			int z = obj->ObjectPoints[vx_id].z;

			unsigned char r = obj->ObjectPointColor[vx_id].r;
			unsigned char g = obj->ObjectPointColor[vx_id].g;
			unsigned char b = obj->ObjectPointColor[vx_id].b;

			// Project 3D coordinates onto 2D screen.
			double new_x = ((double)((x - FRAME_WIDE / 2)*sc) / (z + sc)) + FRAME_WIDE / 2;
			double new_y = ((double)((y - FRAME_HIGH / 2)*sc) / (z + sc)) + FRAME_HIGH / 2;

			POINT3D this_point = { (int)new_x, (int)new_y, z };
			poly_points[v] = this_point;
			poly_colors[v] = { r, g, b };
		}

		// Backface culling.
		//if (backface(poly_points[0], poly_points[1], poly_points[2])) continue;

		// Draw the polygon, clipped by the screen boundaries.
		ClipPolygon(pFrame, zbuff, 0, FRAME_WIDE, FRAME_HIGH, 0, num_v, poly_points, poly_colors);
	
	}

}




// Reads a VJS (vertex join set) file, which describes a 3D object in terms of vertices and polygons.

void read_vjs_file(const char * filename, _Object * object, int * obj_count) {
	FILE * fp;
	char line[300];
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error: cannot open '%s' VJS file.", filename);
		exit(1);
	}
	object->num_points = 0;
	object->num_polys = 0;

	// Get the number of vertices, number of polygons.
	while (strcmp(line, "NVNP") != 0) {
		fgets(line, sizeof(line), fp);
		strtok(line, "\n");
	}
	fgets(line, sizeof(line), fp);
	char * pch = strtok(line, " ,");
	int num_points = 0, num_polys = 0;
	for (int i = 0; i < 2; i++) {
		if (i == 0) num_points = atoi(pch);
		else num_polys = atoi(pch);
		pch = strtok(NULL, " ,\n");
	}

	// Get the vertices stored as (x,y,z) (r,g,b) in the VJS file.
	while (strcmp(line, "VERTICES") != 0) {
		fgets(line, sizeof(line), fp);
		strtok(line, "\n");
	}
	for (int i = 0; i < num_points; i++) {
		fgets(line, sizeof(line), fp);
		// (x,y,z) coordinates.
		pch = strtok(line, " ");
		int x = atoi(pch);
		pch = strtok(NULL, " ");
		int y = atoi(pch);
		pch = strtok(NULL, " ");
		int z = atoi(pch);
		object->ObjectPoints[object->num_points] = { x,y,z };
		// (r,g,b) color values.
		pch = strtok(NULL, " ");
		unsigned char r = atoi(pch);
		pch = strtok(NULL, " ");
		unsigned char g = atoi(pch);
		pch = strtok(NULL, " ");
		unsigned char b = atoi(pch);
		object->ObjectPointColor[object->num_points] = { r,g,b };
		object->num_points++;
	}

	// Get the polygons.
	while (strcmp(line, "POLYGONS") != 0) {
		fgets(line, sizeof(line), fp);
		strtok(line, "\n");
	}
	for (int i = 0; i < num_polys; i++) {
		fgets(line, sizeof(line), fp);
		pch = strtok(line, " ");
		int pcount = 0;
		while (pch != NULL) {
			int point = atoi(pch);
			object->ObjectPolys[object->num_polys].vertex[pcount] = point;
			pcount++;
			pch = strtok(NULL, " \n");
		}
		object->ObjectPolys[object->num_polys].num_vertices = pcount;
		object->num_polys++;
	}
	fclose(fp);


	for (int i = 0; i < object->num_points; i++) {
		POINT3D op = object->ObjectPoints[i];
		VECTOR rp = { (double)op.x, (double)op.y, (double)op.z };
		object->realPoints[i] = rp;

	}

	// Increment the number of active objects.
	*obj_count += 1;


}





