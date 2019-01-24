// 3802 Interactive Graphics Programming and Visulisation.
// 3D Transformations on objects.
// Jack Kearsley, March 2018.

#include "Header.h"


int rnd(double a) {
	return (int)(a + 0.5);
}

void Translate(_Object * obj, VECTOR tv) {
	// For each point in the object.
	int num_pts = obj->num_points;
	for (int i = 0; i < num_pts; i++) {
		VECTOR pt = obj->realPoints[i];
		pt.x += tv.x;
		pt.y += tv.y;
		pt.z += tv.z;
		obj->ObjectPoints[i] = { rnd(pt.x), rnd(pt.y), rnd(pt.z) };
		obj->realPoints[i] = {(double)pt.x, (double)pt.y, (double)pt.z};
	}
}


void Scale(_Object * obj, VECTOR sv) {
	// Incorporate a fixed point so that the object scales without changing position.
	int num_pts = obj->num_points;
	VECTOR fixed = obj->realPoints[0];
	for (int i = 1; i < num_pts; i++) {
		VECTOR pt = obj->realPoints[i];
		pt.x = fixed.x + (pt.x - fixed.x) * (1 + sv.x/100);
		pt.y = fixed.y + (pt.y - fixed.y) * (1 + sv.y/100);
		pt.z = fixed.z + (pt.z - fixed.z) * (1 + sv.z/100);
		obj->ObjectPoints[i] = {rnd(pt.x), rnd(pt.y), rnd(pt.z)};
		obj->realPoints[i] = { (double)pt.x, (double)pt.y, (double)pt.z };
	}
}




void Rotate(_Object * obj, VECTOR rv) {
	
	int num_pts = obj->num_points;

	double A = (rv.x) * PI / 180.0;
	double B = (rv.y) * PI / 180.0;
	double C = (rv.z) * PI / 180.0;

	double mid_x = 0, mid_y = 0, mid_z = 0;

	for (int i = 0; i < num_pts; i++) {
		mid_x += obj->realPoints[i].x;
		mid_y += obj->realPoints[i].y;
		mid_z += obj->realPoints[i].z;
	}
	mid_x /= num_pts;
	mid_y /= num_pts;
	mid_z /= num_pts;

	VECTOR fixed = { mid_x, mid_y, mid_z };


	for (int i = 0; i < num_pts; i++) {
		VECTOR pt = obj->realPoints[i];
		VECTOR op = obj->realPoints[i];

		pt.x -= fixed.x;
		pt.y -= fixed.y;
		pt.z -= fixed.z;

		// x-axis rotation.
		double y = pt.y;
		pt.x = pt.x;
		pt.y = pt.y * cos(A) - pt.z * sin(A);
		pt.z = y * sin(A) + pt.z * cos(A);

		// y-axis rotation.
		double x = pt.x;
		pt.x = pt.z * sin(B) + pt.x * cos(B);
		pt.y = pt.y;
		pt.z = pt.z * cos(B) - x * sin(B);

		// z-axis rotation.
		x = pt.x;
		pt.x = pt.x * cos(C) - pt.y * sin(C);
		pt.y = x * sin(C) + pt.y * cos(C);
		pt.z = pt.z;

		pt.x += fixed.x;
		pt.y += fixed.y;
		pt.z += fixed.z;

		
		obj->ObjectPoints[i] = { rnd(pt.x), rnd(pt.y), rnd(pt.z)};
		obj->realPoints[i] = pt;
	}




}


























