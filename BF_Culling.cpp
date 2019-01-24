// 3802ICT Interactive Graphics Programming and Visualisation.
// Back-face culling.
// Jack Kearsley, March 2018.

#include "Header.h"

VECTOR normalise(VECTOR v1);
double dotProduct(VECTOR v1, VECTOR v2);

bool backface(POINT3D p1, POINT3D p2, POINT3D p3) {
	VECTOR a = {(double) p1.x-p2.x, (double) p1.y-p2.y, (double) p1.z-p2.z};
	VECTOR c = {(double) p3.x-p2.x, (double) p3.y-p2.y, (double) p3.z-p2.z};

	VECTOR N = crossProduct(c, a);

	VECTOR negv0 = {(double) (FRAME_WIDE/2 - p2.x), (double) (FRAME_HIGH/2 - p2.y), (double) -p2.z};
	double res = dotProduct(negv0, N);

	if (res >= 0) return true;
	else return false;












	/*a = normalise(a);
	c = normalise(c);

	double zcross = a.x * c.y - a.y * c.x;

	if (zcross > 0) {
		return true;
	}

	return false;*/
}



VECTOR crossProduct(VECTOR v1, VECTOR v2) {
	VECTOR result = { 0,0,0 };
	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}


double dotProduct(VECTOR v1, VECTOR v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


VECTOR normalise(VECTOR v1) {
	double L = sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
	VECTOR result = { v1.x / L, v1.y / L, v1.z / L };
	return result;
}