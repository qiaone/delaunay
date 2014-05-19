#include <vector>
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include "../Delaunay.h"


Delaunay delaunay;
std::vector<TriMesh::Point> vecPt;
GLfloat width;
GLfloat height;
bool drawMesh = false;

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glPointSize(4);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// draw points
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POINTS);
	int i, n = vecPt.size();
	for (i = 0; i < n; i++)
	{
		glVertex3f(vecPt[i][0], vecPt[i][1], vecPt[i][2]);
	}
	glEnd();

	//draw triangle
	if (drawMesh)
		delaunay.drawMesh();

	glFlush();
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			if (drawMesh)
			{
				vecPt.clear();
				drawMesh = false;
			}
			TriMesh::Point pt(x, height - y, 0);
			vecPt.push_back(pt);
		}

		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{			
			drawMesh = true;
			delaunay.perform(vecPt);
			glutPostRedisplay();
		}
		break;
	}
}


void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Delaunay Triangulation");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}
