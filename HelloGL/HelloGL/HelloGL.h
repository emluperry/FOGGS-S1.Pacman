#pragma once
#include <Windows.h> //required for opengl windows
#include <gl/GL.h> //opengl
#include <gl/GLU.h> //opengl util
#include "GL\freeglut.h" //freeglut lib
#include "GLUTCallbacks.h"

class HelloGL
{
public:
	HelloGL(int argc, char* argv[]); //constructor
	~HelloGL(void); //destructor

	void Display();
	void DrawPolygon();
	void DrawScalene();
	void DrawIsosceles();
	void DrawEquilateral();
	void DrawAcute();
	void DrawRight();
	void DrawObtuse();
	void DrawPentagon();
	void DrawHexagon();
};