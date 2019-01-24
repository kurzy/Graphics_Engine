////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: Header.h
// Author: Jack Kearsley, April 2018.
// Description: Function Prototypes, header files, macros and defines, typedefs and structs.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Turn off warnings for deprecated C functions.
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include "libs\glut.h"
#include <windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "libs/glut32.lib")
#elif __APPLE__
#include <GLUT/glut.h>
#elif __unix__
#include <GL/glut.h>
#endif

//====== Macros and Defines =========
#define FRAME_WIDE	1000
#define FRAME_HIGH	600

#define ROUND(x) ((int)(x+0.5))
#define SQUARE(x) ((x)*(x))
#define PI 3.14159265359
#define ERR_NO_SLOT -9999

#define MAX_NUM_POLYGONS 100
#define POINTS_PER_POLYGON 10
#define MAX_NUM_POINTS 100
#define NUM_MY_OBJECTS 30

#define THRESHOLD_INITIAL 9900


//========= Typedefs and Structs ========
typedef unsigned char BYTE;
struct POINT2D { int x, y; };
struct POINT3D { int x, y, z; };
struct COLOR { unsigned char r, g, b; };
struct VECTOR { double x, y, z; };
struct _Polygon { int vertex[POINTS_PER_POLYGON], num_vertices; };
struct _ObjectAttribs { BYTE r, g, b; POINT3D center, scale; };
struct _Object {
	_ObjectAttribs ObjProps;
	int num_points = 0, num_polys = 0;
	_Polygon ObjectPolys[MAX_NUM_POLYGONS];
	POINT3D ObjectPoints[MAX_NUM_POINTS];
	COLOR ObjectPointColor[MAX_NUM_POINTS];
	VECTOR realPoints[MAX_NUM_POINTS];
};
struct _Animation { VECTOR rotation; VECTOR translation; };
enum side { LEFT, RIGHT, TOP, BOTTOM };

//===== Function Prototypes ========

// 3D_Draw.cpp
void Draw_3D_Object(BYTE * pFrame, int * zbuff, _Object * obj);
void read_vjs_file(const char * filename, _Object * object, int * obj_count);

// main.cpp
void ClearScreen();
void DrawFrame();
void Interlace(BYTE* pL, BYTE* pR);
void PlaySoundEffect(char const * filename);
void BuildFrame(BYTE *pFrame, int * zbuff, int view);
void OnIdle(void);
void OnDisplay(void);
void reshape(int w, int h);
void OnMouse(int button, int state, int x, int y);
void OnKeypress(unsigned char key, int x, int y);
void OnSpecialKeys(int key, int xx, int yy);
void SetPixel(BYTE * pFrame, int x, int y, unsigned char r, unsigned char g, unsigned char b);
int copy_object(_Object * src, _Object * dst, bool * status);
void check_right_portal(_Object * act_obj, bool * status, int * h_points);
void mutate_object(int new_object);

// Line.cpp
void DDA_DrawLine_Shaded(BYTE * pFrame, int * zbuff, POINT3D p1, POINT3D p2, COLOR c1, COLOR c2);
void DDA_DrawLine(BYTE * pFrame, int * zbuff, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b);

// Triangle.cpp
void Triangle(BYTE * pFrame, int * zbuff, POINT3D point[3], COLOR color[3]);

// Polygon.cpp
void Polygon(BYTE * pFrame, int * zbuff, unsigned int n_vertices, POINT3D * vertex, COLOR * color);
int inside(POINT3D p, POINT3D a, POINT3D b, POINT3D c);
int SameSide(POINT3D a, POINT3D b, POINT3D l1, POINT3D l2);
int mod(int a, int b);

// Clipping.cpp
void ClipLine(BYTE * pFrame, int * zbuff, int left, int right, int top, int bottom, POINT3D p1, POINT3D p2, COLOR c);
bool clip_test(double p, double q, double * u1, double * u2);
void ClipPolygon(BYTE * pFrame, int * zbuff, int left, int right, int top, int bottom, const unsigned int n_vertices, POINT3D * vertex, COLOR * color);
void line_bound(enum side clip_line, int line_val, unsigned int * n_vertices, POINT3D * vertex, COLOR * color);
void get_line_intersect(POINT3D p1, POINT3D p2, COLOR c1, COLOR c2, enum side clip_line, int line_val, POINT3D * intersect, COLOR * intersect_color);

// Transform.cpp
void Translate(_Object * obj, VECTOR tv);
void Scale(_Object * obj, VECTOR sv);
void Rotate(_Object * obj, VECTOR rv);
int rnd(double a);

// BF_Culling.cpp
bool backface(POINT3D p1, POINT3D p2, POINT3D p3);
VECTOR crossProduct(VECTOR a, VECTOR b);

// Mouse_Actions.cpp
bool process_click(_Object * obj, bool * status, int x, int y);
POINT3D ProjectTo2D(POINT3D a);

// Paint.cpp
void Paint_background(BYTE * pFrame, int * zbuff);
void Paint_healthbar(BYTE * pFrame, int * zbuff, int h_points);
void Paint_text(char const * text, COLOR c, int x, int y, void * fnt);
void Paint_scoreboard(BYTE * pFrame, int * zbuff);



//////////////////////////////////////////////////////////////////
// END OF FILE
/////////////////////////////////////////////////////////////////