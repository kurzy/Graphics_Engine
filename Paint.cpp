// 3802ICT Interactive Graphics Programming and Visualisation.
// Functions for painting graphics to the screen.
// Jack Kearsley, March 2018.

#include "Header.h"

//===== Global Variables ========
// If the stars have already been given positions.
bool stars_set = false;

//====== Defines =============
#define SPACE_STARS_COUNT 500

// Randomly generates the positions of stars in space (which are stored in the 'stars' POINT3D array).
void generate_stars(POINT2D * stars) {
	// Draw some random stars (white dots) in the top and bottom triangles.
	for (int i = 0; i < SPACE_STARS_COUNT; i++) {
		// Get a random y-point.
		int rand_y = rand() % FRAME_HIGH;
		static double dy = (FRAME_HIGH) / (double)(FRAME_WIDE);
		// Calculate the upper and lower x points, so that it lands within the bottom two triangles (and not on top of the portals).
		int x_lower, x_upper;
		if (rand_y > FRAME_HIGH / 2) {
			x_upper = rand_y / dy;
			x_lower = FRAME_WIDE - x_upper;
		}
		else {
			x_lower = rand_y / dy;
			x_upper = FRAME_WIDE - x_lower;
		}
		// Now choose a random x-point between the upper and lower bounds.
		int rand_x = rand() % (x_upper + 1 - x_lower) + x_lower;
		stars[i] = { rand_x, rand_y };
	}
	stars_set = true;
}


// Paint the background (blue and red portals, stars).
void Paint_background(BYTE * pFrame, int * zbuff) {
	// Colors for left and right side portals.
	COLOR blue1 = { 27,6,232 };
	COLOR blue2 = { 104,7,255 };
	COLOR red1 = { 255,155,1 };
	COLOR red2 = { 255,27,2 };
	COLOR space_blue = { 0,1,71 };
	COLOR space_red = { 71,7,0 };
	COLOR black = { 20, 20, 20 };

	// Top-half black triangle (space).
	POINT3D tri3[3] = { { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000 },{ FRAME_WIDE - 1, FRAME_HIGH - 1, 0 },{ 0, FRAME_HIGH - 1,0 } };
	COLOR col3[3] = { black, space_red, space_blue };
	Triangle(pFrame, zbuff, tri3, col3);

	// Bottom-half black triangle (space).
	POINT3D tri4[3] = { { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000 },{ 0,0,0 },{ FRAME_WIDE - 1, 0,0 } };
	COLOR col4[3] = { black, space_blue, space_red };
	Triangle(pFrame, zbuff, tri4, col4);
	
	// Generate some stars at random positions (within the top and bottom space triangles).
	static POINT2D stars[SPACE_STARS_COUNT];
	static bool twinkle[SPACE_STARS_COUNT];
	if (!stars_set) generate_stars(stars);
	// Some stars will be brighter (twinkle == true). Re-assign the twinkling stars randomly.
	if (rand() % 1000 > 500) {
		for (int i = 0; i < SPACE_STARS_COUNT; i++) {
			if (rand() % 100 > 98)
				twinkle[i] = !twinkle[i];
		}
	}
	COLOR star_color = { 150, 100, 100 };
	COLOR star_color_bright = { 255, 255, 255 };
	// Draw the stars.
	for (int i = 0; i < SPACE_STARS_COUNT; i++) {
		COLOR c = (twinkle[i]) ? star_color_bright : star_color;
		SetPixel(pFrame, stars[i].x, stars[i].y, c.r, c.g, c.b);
	}

	// Paint left (blue) portal.
	POINT3D tri1[3] = { { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000 },{ 0,0,0 },{ 0,FRAME_HIGH, 0 } };
	COLOR col1[3] = { black, blue1, blue2 };
	Triangle(pFrame, zbuff, tri1, col1);

	// Paint right (red) portal.
	POINT3D tri2[3] = { { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000 },{ FRAME_WIDE - 1, FRAME_HIGH - 1, 0 },{ FRAME_WIDE - 1, 0,0 } };
	COLOR col2[3] = { black,red1, red2 };
	Triangle(pFrame, zbuff, tri2, col2);

	// Paint an outline around the left and right portals.
	COLOR portal_outline = { 255,255,255 };
	DDA_DrawLine_Shaded(pFrame, zbuff, { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000-10 }, { 0,0,-100 }, { 10,10,10 }, portal_outline);
	DDA_DrawLine_Shaded(pFrame, zbuff, { FRAME_WIDE / 2, FRAME_HIGH / 2 , 2000-10 }, { FRAME_WIDE-4, 0, -100 }, { 10,10,10 }, portal_outline);
	DDA_DrawLine_Shaded(pFrame, zbuff, { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000-10 }, { FRAME_WIDE-2, FRAME_HIGH, -100 }, { 10,10,10 }, portal_outline);
	DDA_DrawLine_Shaded(pFrame, zbuff, { FRAME_WIDE / 2, FRAME_HIGH / 2, 2000-10 }, { 0, FRAME_HIGH, -100 }, { 10,10,10 }, portal_outline);
}


// Draw the health bar to the bottom of the screen.
void Paint_healthbar(BYTE * pFrame, int * zbuff, int h_points) {
	// If no health left, nothing to paint.
	if (h_points <= 0) return;
	double h = h_points;
	// The length of the bar represents the player's remaining health.
	int length = 600 * (h / 10);
	int width = 7;
	int start = 200;
	// As the bar changes in length the color gradient should remain constant, so the right color must be recalculated.
	double r = 255 * (1-(h / 10));
	double g = 255 - 255 * (1-(h / 10));
	COLOR right_color = { (BYTE)r, (BYTE)g, 0 };

	for (int i = 0; i < width; i++) {
		DDA_DrawLine_Shaded(pFrame, zbuff, { start,i,0 }, { start + length,i,0 }, { 255,0,0 }, right_color);
	}
}

// Paints some text to the screen, in a specified color, at the (x, y) position.
void Paint_text(char const * text, COLOR c, int x, int y, void * fnt) {
	glColor3f((double)c.r/255, (double)c.g/255, (double)c.b/255);
	glRasterPos2f(x, y);
	for (char const * c = text; *c != '\0'; c++) {
		glutBitmapCharacter(fnt, *c);
	}
}


// Paints a box around the scoreboard.
void Paint_scoreboard(BYTE * pFrame, int * zbuff) {
	DDA_DrawLine_Shaded(pFrame, zbuff, {440, FRAME_HIGH-10, 0}, {570, FRAME_HIGH-10, 0}, {255,255,255}, {255,255,255});
	DDA_DrawLine_Shaded(pFrame, zbuff, {440, FRAME_HIGH-40, 0}, {570, FRAME_HIGH-40, 0}, { 255,255,255 }, { 255,255,255 });
	DDA_DrawLine_Shaded(pFrame, zbuff, {440, FRAME_HIGH-10, 0}, {440, FRAME_HIGH-40, 0}, { 255,255,255 }, { 255,255,255 });
	DDA_DrawLine_Shaded(pFrame, zbuff, {570, FRAME_HIGH-10, 0}, {570, FRAME_HIGH-40, 0}, { 255,255,255 }, { 255,255,255 });
}



////////////////////////////////////////////////////////////////
// END OF FILE
////////////////////////////////////////////////////////////////