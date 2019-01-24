// 3802ICT Interactive Graphics Programming and Visualisation.
// Drawing 2D shaded-colour triangles.
// Jack Kearsley, March 2018.

#include "Header.h"


void Triangle(BYTE * pFrame, int * zbuff, POINT3D point[3], COLOR color[3]) {
	//int Ledge[FRAME_HIGH+10], Redge[FRAME_HIGH+10], Lzval[FRAME_HIGH+10], Rzval[FRAME_HIGH+10];

	int * Ledge = (int *)calloc(FRAME_HIGH+10, sizeof(int));
	int * Redge = (int *)calloc(FRAME_HIGH + 10, sizeof(int));
	int * Lzval = (int *)calloc(FRAME_HIGH + 10, sizeof(int));
	int * Rzval = (int *)calloc(FRAME_HIGH + 10, sizeof(int));


	bool used[3] = { false, false, false };

	// Dealing with flat-top triangles.
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (point[i].y == point[j].y && i != j) point[i].y += 1;
		}
	}

	// Get highest point based on y-value.
	POINT3D max = { 0,0,0 };
	COLOR max_color = { 0,0,0 };
	int idx = 0;
	for (int i = 0; i < 3; i++) {
		if (point[i].y > max.y) {
			max = point[i];
			max_color = color[i];
			idx = i;
		}
	}
	used[idx] = true;

	// Left vertex forms the smallest angle with the max vertex.
	POINT3D left = { 0,0,0 };
	COLOR left_color = { 0,0,0 };
	double min_angle = 10.0;
	for (int i = 0; i < 3; i++) {
		if (!used[i]) {
			double a = atan2(max.y - point[i].y, max.x - point[i].x);
			if (a < min_angle) {
				left = point[i];
				left_color = color[i];
				min_angle = a;
				idx = i;
			}
		}
	}
	used[idx] = true;

	// Right vertex (the 3rd remaining vertex).
	POINT3D right;
	COLOR right_color = { 0,0,0 };
	for (int i = 0; i < 3; i++) {
		if (!used[i]) {
			right = point[i];
			right_color = color[i];
		}
	}

	// Get the x points for the scan lines.
	double mL = (max.x - left.x) / ((double)(max.y - left.y));
	double mR = (max.x - right.x) / ((double)(max.y - right.y));

	double mz_left = (max.z - left.z) / (double)(max.y - left.y);
	double mz_right = (max.z - right.z) / (double)(max.y - right.y);
	double zL = max.z;
	double zR = max.z;

	double xL = max.x;
	double xR = max.x;
	int hL = abs(left.y - max.y);
	int hR = abs(right.y - max.y);
	int high = (hL > hR) ? hL : hR;

	for (int y = 0; y < high; y++) {
		Ledge[y] = xL - 1.0;
		Redge[y] = xR + 1.0;

		Lzval[y] = zL - 1.0;
		Rzval[y] = zR + 1.0;

		if (y == hL) {
			mL = (right.x - left.x) / (double)(right.y - left.y);
			mz_left = (right.z - left.z) / (double)(right.y - left.y);
		}
		if (y == hR) {
			mR = (left.x - right.x) / (double)(left.y - right.y);
			mz_right = (left.z - right.z) / (double)(left.y - right.y);
		}
		xL -= mL;
		xR -= mR;
		zL -= mz_left;
		zR -= mz_right;

	}

	// Do the left color values.
	//COLOR Lcolr[FRAME_HIGH+10];
	COLOR * Lcolr = (COLOR *)calloc(FRAME_HIGH + 10, sizeof(COLOR));
	COLOR this_color = max_color;
	// RGB gradients as we step from 'max_color' to 'left_color'.
	double y_diff = (double)(max.y - left.y);
	double m_r = (left_color.r - max_color.r) / y_diff;
	double m_g = (left_color.g - max_color.g) / y_diff;
	double m_b = (left_color.b - max_color.b) / y_diff;

	double dec_r = this_color.r;
	double dec_g = this_color.g;
	double dec_b = this_color.b;

	const int abs_high = abs(high);

	for (int i = 0; i < abs_high; i++) {
		Lcolr[i] = this_color;
		if (i == abs(hL)) {
			double LRdiff = (double)(left.y - right.y);
			m_r = (right_color.r - left_color.r) / LRdiff;
			m_g = (right_color.g - left_color.g) / LRdiff;
			m_b = (right_color.b - left_color.b) / LRdiff;
		}
		dec_r += m_r;
		dec_g += m_g;
		dec_b += m_b;
		this_color.r = dec_r;
		this_color.g = dec_g;
		this_color.b = dec_b;
	}


	// Do the right color values.
	//COLOR Rcolr[FRAME_HIGH+10];
	COLOR * Rcolr = (COLOR *)calloc(FRAME_HIGH + 10, sizeof(COLOR));

	this_color = max_color;
	// RGB gradients as we step from 'max_color' to 'right_color'.
	y_diff = (double)(max.y - right.y);
	m_r = (right_color.r - max_color.r) / y_diff;
	m_g = (right_color.g - max_color.g) / y_diff;
	m_b = (right_color.b - max_color.b) / y_diff;

	dec_r = this_color.r;
	dec_g = this_color.g;
	dec_b = this_color.b;

	for (int i = 0; i < abs(high); i++) {
		Rcolr[i] = this_color;

		if (i == abs(hR)) {
			double LRdiff = (double)(right.y - left.y);
			m_r = (left_color.r - right_color.r) / LRdiff;
			m_g = (left_color.g - right_color.g) / LRdiff;
			m_b = (left_color.b - right_color.b) / LRdiff;
		}

		dec_r += m_r;
		dec_g += m_g;
		dec_b += m_b;
		this_color.r = dec_r;
		this_color.g = dec_g;
		this_color.b = dec_b;
	}

	// Paint the pixels.
	for (int i = 0; i < abs(high); i++) {
		DDA_DrawLine_Shaded(pFrame, zbuff, { Ledge[i], abs(max.y) - i, Lzval[i] }, { Redge[i], abs(max.y) - i, Rzval[i] }, Lcolr[i], Rcolr[i]);
	}

	free(Ledge);
	free(Redge);
	free(Lzval);
	free(Rzval);
	free(Lcolr);
	free(Rcolr);


}


