#include "HelloGL.h"

HelloGL::HelloGL(int argc, char* argv[])
{
	GLUTCallbacks::Init(this);
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple OpenGL Program");
	glutDisplayFunc(GLUTCallbacks::Display);
	glutMainLoop();
}

void HelloGL::Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	DrawPolygon();
	DrawRight();
	DrawAcute();
	DrawIsosceles();
	DrawObtuse();
	DrawScalene();
	DrawEquilateral();
	DrawPentagon();
	DrawHexagon();
	glFlush();
}

void HelloGL::DrawPolygon()
{
	glBegin(GL_POLYGON);
	{
		glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
		glVertex2f(-0.75, 0.5);
		glVertex2f(0.75, 0.5);
		glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		glVertex2f(0.75, -0.5);
		glVertex2f(-0.75, -0.5);
		glEnd();
	}
}

void HelloGL::DrawScalene()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.0f, 0.1f, 0.2f);
		glVertex2f(-0.3, 0.5);
		glVertex2f(0.0f, 0.1f);
		glVertex2f(-0.20, -0.5f);
		glEnd();
	}
}

void HelloGL::DrawIsosceles()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(-0.5, 0.5);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(-0.50, -0.5f);
		glEnd();
	}
}

void HelloGL::DrawEquilateral()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex2f(-0.5, 0.35);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(-0.50, -0.35f);
		glEnd();
	}
}

void HelloGL::DrawAcute()
{
	glBegin(GL_POLYGON);
	{
		glColor4f(1.0f, 0.5f, 0.0f, 0.0f);
		glVertex2f(-0.2, 0.5);
		glVertex2f(0.50, -0.5);
		glVertex2f(-0.50, -0.5);
		glEnd();
	}
}

void HelloGL::DrawRight()
{
	glBegin(GL_POLYGON);
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex2f(-0.5, 0.5);
		glVertex2f(0.50, -0.5);
		glVertex2f(-0.50, -0.5);
		glEnd();
	}
}

void HelloGL::DrawObtuse()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.5f, 1.0f, 1.0f);
		glVertex2f(-0.8, 0.5);
		glVertex2f(0.50, -0.5);
		glVertex2f(-0.50, -0.5);
		glEnd();
	}
}

void HelloGL::DrawPentagon()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.0f, 0.5f, 0.5f);
		glVertex2f(0.0, 0.5);
		glVertex2f(0.5, 0.2);
		glVertex2f(0.3, -0.3);
		glVertex2f(-0.3, -0.3);
		glVertex2f(-0.5, 0.2);
		glEnd();
	}
}

void HelloGL::DrawHexagon()
{
	glBegin(GL_POLYGON);
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.3, 0.2);
		glVertex2f(0.6, 0.0);
		glVertex2f(0.3, -0.2);
		glVertex2f(-0.3, -0.2);
		glVertex2f(-0.6, 0.0);
		glVertex2f(-0.3, 0.2);
		glEnd();
	}
}

HelloGL::~HelloGL(void)
{

}