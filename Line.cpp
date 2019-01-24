// 3802ICT Interactive Graphics Programming and Visualisation.
// Drawing shaded lines.
// Jack Kearsley, March 2018.


#include "Header.h"


// Old function - use DDA_DrawLine_Shaded() instead.
void DDA_DrawLine(BYTE * pFrame, int * zbuff, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int steps;

	bool stepping_vertical = true;
	if (abs(dx) > abs(dy)) {
		steps = abs(dx);
		stepping_vertical = false;
	}
	else steps = abs(dy);
	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;
	double x = x1;
	double y = y1;

	SetPixel(pFrame, ROUND(x), ROUND(y), r, g, b);
	for (int i = 0; i < steps; i++, x += x_inc, y += y_inc) {
		SetPixel(pFrame, ROUND(x), ROUND(y), r, g, b);
	}

}


void DDA_DrawLine_Shaded(BYTE * pFrame, int * zbuff, POINT3D p1, POINT3D p2, COLOR c1, COLOR c2) {
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	int dz = p2.z - p1.z;
	int steps;
	if (abs(dx) > abs(dy)) {
		steps = abs(dx);
	}
	else steps = abs(dy);
	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;
	double z_inc = dz / (double)steps;
	double x = p1.x;
	double y = p1.y;
	double z = p1.z;
	double length = sqrt((double)(SQUARE(p2.x - p1.x) + SQUARE(p2.y - p1.y)));
	double rdiff = (c2.r - c1.r) / length;
	double gdiff = (c2.g - c1.g) / length;
	double bdiff = (c2.b - c1.b) / length;
	double rval = (double)c1.r;
	double gval = (double)c1.g;
	double bval = (double)c1.b;

	for (int i = 0; i < steps; i++) {
		int f_index = ROUND(y) * FRAME_WIDE + ROUND(x);
		if (ROUND(z) < zbuff[f_index]) {
			SetPixel(pFrame, ROUND(x), ROUND(y), (unsigned char)rval, (unsigned char)gval, (unsigned char)bval);
			zbuff[f_index] = ROUND(z);
		}
		x += x_inc;
		y += y_inc;
		z += z_inc;
		rval += rdiff;
		gval += gdiff;
		bval += bdiff;
	}
}