////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
// Author: Jack Kearsley, April 2018.
// Description: Program entry point, GLUT operations, buildFrame function.
// The aim of the game is to shoot down flying 3D objects with the mouse, before they pass through the portal on the right
// side of the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Header.h"
#include <math.h>
#include <time.h>
#include <limits.h>

//====== Global Variables ==========
BYTE	pFrameL[FRAME_WIDE * FRAME_HIGH * 3];
BYTE	pFrameR[FRAME_WIDE * FRAME_HIGH * 3];
bool	stereo = false;
int		eyes = 10;

// Object library preserves the original 3D objects from the VJS files.
_Object * object_library;
int object_library_count = 0;
// Active objects stores the 3D objects that are currently being modified and animated across the screen.
_Object * active_objects;
// Keep track of which 'active_objects' slots are available for use.
bool active_objects_status[NUM_MY_OBJECTS];
// Stores the characteristics of each object's movement across the screen (translation, rotation, speed of animation).
_Animation active_objects_animation[NUM_MY_OBJECTS];
// The player's score (how many objects they have shot down in the current game).
int player_score = 0;
// The threshold for whether to create a new, random 3D object to fly across the screen, decreases as the game progresses.
int threshold = THRESHOLD_INITIAL;

// The player's health points, if it reaches 0 the player has lost and the game is over.
int health_points = 10;
bool game_over = false;
bool game_over_sound = false;

// Before the user plays, the title screen displays.
bool title_screen = true;
bool title_screen_sound = false;

// Allows the difficulty to be incremented each second. Keeps track of time since last difficulty increase.
time_t start_time;

// Z Buffering array.
int zbuff[FRAME_WIDE * (FRAME_HIGH+1)];


////////////////////////////////////////////////////////
// Program Entry Point
////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	printf("Portal Protector Game, by Jack Kearsley, April 2018.\n");
	object_library = (_Object *)calloc(NUM_MY_OBJECTS, sizeof(_Object));
	active_objects = (_Object *)calloc(NUM_MY_OBJECTS, sizeof(_Object));
	for (int i = 0; i < NUM_MY_OBJECTS; i++) active_objects_status[i] = false;
	srand(time(NULL));

	//-- setup GLUT --
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	//GLUT_3_2_CORE_PROFILE |
	glutInitWindowSize(FRAME_WIDE, FRAME_HIGH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glutSetWindowTitle("Portal Protector Game");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	/*
	#ifdef WIN32
	//- eliminate flashing --
	typedef void (APIENTRY * PFNWGLEXTSWAPCONTROLPROC) (int i);
	PFNWGLEXTSWAPCONTROLPROC wglSwapControl = NULL;
	wglSwapControl = (PFNWGLEXTSWAPCONTROLPROC) wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapControl != NULL) wglSwapControl(1);
	#endif
	*/

	// Read in VJS files to the object_library.
	read_vjs_file("vjs_files/cube.txt", &object_library[object_library_count], &object_library_count);
	read_vjs_file("vjs_files/pyramid.txt", &object_library[object_library_count], &object_library_count);
	read_vjs_file("vjs_files/diamond.txt", &object_library[object_library_count], &object_library_count);

	//--- set openGL state --
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//-- register call back functions --
	glutIdleFunc(OnIdle);
	glutDisplayFunc(OnDisplay);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(OnKeypress);
	glutMouseFunc(OnMouse);

	glutSpecialFunc(OnSpecialKeys);
	//-- run the program
	glutMainLoop();

	free(object_library);
	free(active_objects);
	return 0;
}


// Copies the all of the data from one '_Object' struct to another (deep copy).
// Returns an int which is the slot in 'active_objects' that the source object was copied to.

int copy_object(_Object * src, _Object * act_obj, bool * status) {
	
	// Find an empty slot in 'active_objects'.
	int slot = 0;
	bool found_slot = false;
	for (int i = 0; i < NUM_MY_OBJECTS; i++) {
		if (!status[i]) {
			slot = i;
			found_slot = true;
			status[i] = true;
			break;
		}
	}
	// If no slot is free, return an error flag.
	if (!found_slot) return ERR_NO_SLOT;

	// Copy over number of points, and number of polygons.
	_Object * dst = &act_obj[slot];
	dst->num_points = src->num_points;
	dst->num_polys = src->num_polys;

	// Copy over the polygons.
	for (int p = 0; p < src->num_polys; p++) {
		int num_v = src->ObjectPolys[p].num_vertices;
		for (int v = 0; v < num_v; v++) {
			int vert_no = src->ObjectPolys[p].vertex[v];
			dst->ObjectPolys[p].vertex[v] = vert_no;
		}
		dst->ObjectPolys[p].num_vertices = num_v;
	}

	// Copy vertex data.
	for (int v = 0; v < src->num_points; v++) {
		POINT3D pt = src->ObjectPoints[v];
		COLOR col = src->ObjectPointColor[v];
		VECTOR vc = src->realPoints[v];
		dst->ObjectPoints[v] = pt;
		dst->ObjectPointColor[v] = col;
		dst->realPoints[v] = vc;
	}
	
	return slot;
}


////////////////////////////////////////////////////////
// Event Handers
////////////////////////////////////////////////////////

void OnIdle(void)
{
	DrawFrame();
	glutPostRedisplay();
}


// GLUT Text must be painted in this function, and not in BuildFrame(), because the pixels are overwritten by glDrawPixels().

void OnDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(FRAME_WIDE, FRAME_HIGH, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)pFrameR);

	// If the player is on the title screen, paint some text.
	if (title_screen) {
		if (!title_screen_sound) {
			PlaySoundEffect("audio/start_menu.wav");
			title_screen_sound = true;
		}
		Paint_text("Portal Protector", { 200, 100, 0 }, 450, 400, GLUT_BITMAP_HELVETICA_18);
		Paint_text("Shoot down the flying space junk with your mouse, before it reaches the red portal.", {255,255,255}, 200, 300, GLUT_BITMAP_HELVETICA_18);
		Paint_text("Press Enter To Play...", { 255,255,255 }, 430, 200, GLUT_BITMAP_HELVETICA_18);
		Paint_text("Created by Jack Kearsley, April 2018.", {100,100,100}, 430, 100, GLUT_BITMAP_HELVETICA_10);
	}
	// If the game is currently being played, or the game is lost.
	else {
		// Paint the score number.
		char score_str[10];
		sprintf(score_str, "%d", player_score);
		Paint_text(score_str, { 255,255,255 }, 500, FRAME_HIGH - 30, GLUT_BITMAP_HELVETICA_18);
		
		// If the player has lost the game.
		if (game_over) {
			Paint_text("Game Over", { 255,0,0 }, 450, 400, GLUT_BITMAP_HELVETICA_18);
			Paint_text("Press Enter For New Game...", { 255,255,255 }, 400, 300, GLUT_BITMAP_HELVETICA_18);
		}
	}

	glutSwapBuffers();
	glFlush();
}



void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// When mouse button is pressed 
void OnMouse(int button, int state, int x, int y) {
	// Don't perform actions twice when mouse is clicked, only on mouse down.
	if (state == GLUT_UP) return;
	// Make the origin at the bottom-left corner.
	y = FRAME_HIGH - y;
	x += 2;
	
	// See if the click was inside of a 3D object.
	if (process_click(active_objects, active_objects_status, x, y)) {		
		// Randomly choose from 6 sound effects (shoot0..5.wav).
		int r = rand() % 6;
		char fname[50];
		sprintf(fname, "audio/shoot%d.wav", r);
		PlaySoundEffect(fname);
		player_score++;
	}

}



// On keyboard character press.
void OnKeypress(unsigned char key, int x, int y)
{
	switch(key) {
	// Enter key.
	case 13:
		// If enter is pressed on the title screen, it begins the game.
		if (title_screen) {
			title_screen = false;
			start_time = time(NULL);
			break;
		}
		// If Enter is pressed when the game is over, reset some variables and begin a new game.
		if (!game_over) break;
		game_over = false;
		game_over_sound = false;
		health_points = 10;
		player_score = 0;
		start_time = time(NULL);
		threshold = THRESHOLD_INITIAL;
		for (int i = 0; i < NUM_MY_OBJECTS; i++) active_objects_status[i] = false;
		break;
	// Z key for stereo toggling.
	case 'z':
		stereo = stereo ? 0 : 1;
		break;
	}

}


// On special key press (eg. arrow keys).
void OnSpecialKeys(int key, int xx, int yy) {


}

////////////////////////////////////////////////////////
// Utility Functions
////////////////////////////////////////////////////////

void ClearScreen()
{
	memset(pFrameL, 0, FRAME_WIDE * FRAME_HIGH * 3);
	memset(pFrameR, 0, FRAME_WIDE * FRAME_HIGH * 3);
	for (int i = 0; i < (FRAME_HIGH * FRAME_WIDE); i++) zbuff[i] = INT_MAX;
}


// Interlace the pFrameL and pFrameR buffers for stereo mode.
void Interlace(BYTE* pL, BYTE* pR)
{
	int rowlen = 3 * FRAME_WIDE;
	for (int y = 0; y < FRAME_HIGH; y += 2)
	{
		for (int x = 0; x < rowlen; x++) {
			*pR++ = *pL++;
		}
		pL += rowlen;
		pR += rowlen;
	}
}


void DrawFrame()
{
	ClearScreen();
	if (!stereo) {
		BuildFrame(pFrameR, zbuff, 0);
	}
	else {
		BuildFrame(pFrameL, zbuff, -eyes);
		// Reset the zbuffer for use with BuildFrame(pFrameR).
		for (int i = 0; i < (FRAME_HIGH * FRAME_WIDE); i++) zbuff[i] = INT_MAX;
		BuildFrame(pFrameR, zbuff, eyes);
		Interlace((BYTE*)pFrameL, (BYTE*)pFrameR);
	}

}


void	PlaySoundEffect(char const * filename)
{
#ifdef _WIN32
	PlaySoundA(filename, NULL, SND_ASYNC | SND_FILENAME);
#else
	char command[80];
#ifdef __APPLE__
	sprintf(command, "afplay %s &", filename);
#else
	sprintf(command, "play %s &", filename);
#endif	
	system(command);
#endif
}


// Builds the pixel array. Set as the idle function for GLUT.

void BuildFrame(BYTE *pFrame, int * zbuff, int view)
{
	// Use copy of screen pointer for safety.
	BYTE*	screen = (BYTE*)pFrame;
	// If the user is still on the title screen, don't start the game yet.
	if (title_screen) return;

	// If game over, play the 'game_over' sound once, and return.
	if (game_over) {
		if (!game_over_sound) {
			PlaySoundEffect("audio/game_over.wav");
			game_over_sound = true;
		}
		return;
	}

	// Paint the background, health bar and scoreboard.
	Paint_background(screen, zbuff);
	Paint_healthbar(screen, zbuff, health_points);
	Paint_scoreboard(screen, zbuff);

	// Modify the x points for stereo display mode (increment each x point by 'view').
	if (stereo) {
		// For each object in 'active_objects'.
		for (int o = 0; o < NUM_MY_OBJECTS; o++) {
			// If the object slot isn't active, don't modify it.
			if (!active_objects_status[o]) continue;
			int n_points = active_objects[o].num_points;
			for (int i = 0; i < n_points; i++) {
				active_objects[o].ObjectPoints[i].x += view;
			}
		}
	}

	// A new 3D object may be created depending on a probability.
	// The threshold of new objects appearing increases each second (so the game gets harder).
	if (difftime(time(NULL), start_time) > 1) {
		threshold -= 2;
		start_time = time(NULL);
	}

	// If a random number is above the threshold, create a random 3D object that flies across the screen.
	if (rand() % 10000 > threshold) {
		int rand_object_no = rand() % object_library_count;
		int new_object = copy_object(&object_library[rand_object_no], active_objects, active_objects_status);
		// If the copy_object operation succeeded, apply some random transformations to the object.
		if (new_object != ERR_NO_SLOT) {
			mutate_object(new_object);
		}
	}

	// Draw the active 3D objects.
	for (int o = 0; o < NUM_MY_OBJECTS; o++) {
		if (!active_objects_status[o]) continue;
		Draw_3D_Object(screen, zbuff, &active_objects[o]);
		Translate(&active_objects[o], active_objects_animation[o].translation);
		Rotate(&active_objects[o], active_objects_animation[o].rotation);
	}

	// Reset the x points for stereo display mode (by subrtracting 'view' from all x points).
	if (stereo) {
		// For each object in 'object_library'.
		for (int o = 0; o < NUM_MY_OBJECTS; o++) {
			if (!active_objects_status[o]) continue;
			int n_points = active_objects[o].num_points;
			for (int i = 0; i < n_points; i++) {
				active_objects[o].ObjectPoints[i].x -= view;
			}
		}
	}
	
	// Check for objects that have touched the right side portal.
	// For each object that touches the portal, the user loses a health point.
	check_right_portal(active_objects, active_objects_status, &health_points);

	// If no health points left, the user has lost the game, and is presented with the 'game over' screen.
	if (health_points <= 0) game_over = true;

}



// Set a pixel's RGB color value in the 'pFrame' pixel array.
// Pixel's position specified by (x, y).
void SetPixel(BYTE * pFrame, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	int loc = (x + y*FRAME_WIDE) * 3;
	pFrame[loc] = r;
	pFrame[loc + 1] = g;
	pFrame[loc + 2] = b;
}


// mutate_object adds some characteristics to a newly created object (color, animation, translation).

void mutate_object(int new_object) {
	// Perform an initial translation.
	int lower_z_bound = -100;
	int upper_z_bound = 1500;
	double rand_z = rand() % (upper_z_bound + 1 - lower_z_bound) + lower_z_bound;
	Translate(&active_objects[new_object], { -rand_z,0,rand_z });

	// Perform an initial rotation.
	double ro_x = rand() % 360;
	double ro_y = rand() % 360;
	double ro_z = rand() % 360;
	Rotate(&active_objects[new_object], { ro_x, ro_y, ro_z });

	// Perform an initial scale on x-axis.
	double sc_x = rand() % 100;
	Scale(&active_objects[new_object], { sc_x, 0,0 });

	// Give the object some animation characteristics (translation and rotation).
	double tr_x = rand() % (20 + 1 - 10) + 10;
	active_objects_animation[new_object].translation = { tr_x, 0,0 };
	ro_x = rand() % 20;
	ro_y = rand() % 20;
	ro_z = rand() % 20;
	active_objects_animation[new_object].rotation = { ro_x, ro_y, ro_z };

	// Give the object's vertices random colors.
	for (int v = 0; v < active_objects[new_object].num_points; v++) {
		unsigned char rand_r = rand() % 256;
		unsigned char rand_g = rand() % 256;
		unsigned char rand_b = rand() % 256;
		active_objects[new_object].ObjectPointColor[v] = {rand_r, rand_g, rand_b};
	}

}




// Check if any objects have collided with the right portal.
// If an object collides with the right portal, decrement the health points by 1.

void check_right_portal(_Object * act_obj, bool * status, int * h_points) {
	// Change in z from the middle of screen to the edge of screen (for both left and right portals).
	static double dz = -2000 / (FRAME_WIDE / 2);

	// For each active object.
	for (int o = 0; o < NUM_MY_OBJECTS; o++) {
		if (!status[o]) continue;

		// For each vertex in the object.
		for (int v = 0; v < act_obj[o].num_points; v++) {
			POINT3D pt = ProjectTo2D(act_obj[o].ObjectPoints[v]);
			// Modify the x point slightly so the object vanishes AFTER crossing through the portal, not when it first intersects it.
			// Proportional to z value as objects closer to the viewer need greater x offset.
			pt.x -= (2000-pt.z)/10 ;

			// The portal surface's z value for a given x value, using dz.
			double z_intersect_point = 2000 + (pt.x - FRAME_WIDE / 2) * dz;
			// If the object has passed through the right portal.
			if (pt.z > z_intersect_point) {
				status[o] = false;
				*h_points -= 1;
				return;
			}

		}

	}//Active_objects

}





