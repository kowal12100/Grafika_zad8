// Damian Moska�a

// Program 1 - Wariant pierwszy wykorzystuj�cy bufor szablonowy do konstruowania obiekt�w przy u�yciu operacji CSG
#include "stdafx.h"
#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"
#define _USE_MATH_DEFINES
#include <cmath>

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif 

enum
{
	SPHERE,
	PYRAMID,
	TORUS,
	TORUS_AND_SPHERE,
	PYRADMID_SUB_SPHERE,
	OR,
	FULL_WINDOW,
	ASPECT_1_1,
	EXIT
};

GLint option;
int aspect = FULL_WINDOW;
const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;
int button_state = GLUT_UP;
int button_x, button_y;
GLint Torus, Sphere, Pyramid, TorusANDSphere, PyramidSubSphere, Or;
const int N = 9;
const int VertexNumber = N + 2;
const int TrianglesNumber = N * 2;
GLfloat vertex[VertexNumber * 3];
GLint triangles[TrianglesNumber * 3];

void GenerateVerticles(GLfloat* vertices, int Sides)
{
	for (auto i = 0; i < Sides; ++i)
	{
		vertices[i * 3 + 0] = sinf(i / static_cast<float>(Sides) * 2 * M_PI);
		vertices[i * 3 + 1] = -1.2f;
		vertices[i * 3 + 2] = cosf(i / static_cast<float>(Sides) * 2 * M_PI);
	}

	int l = Sides * 3;
	vertices[l + 0] = 0.f;
	vertices[l + 1] = -1.2f;
	vertices[l + 2] = 0.f;
	vertices[l + 3] = 0.f;
	vertices[l + 4] = 1.0f;
	vertices[l + 5] = 0.f;
}
void GenerateTriangles(GLint* triangles, int Sides)
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < Sides; ++j)
		{
			triangles[i*Sides * 3 + j * 3] = j;
			triangles[i*Sides * 3 + j * 3 + 1] = (j + 1) % Sides;
			triangles[i*Sides * 3 + j * 3 + 2] = Sides + i;
		}
	}
}

void Normal(GLfloat *n, int i)
{
	GLfloat v1[3], v2[3];
	v1[0] = vertex[3 * triangles[3 * i + 1] + 0] - vertex[3 * triangles[3 * i + 0] + 0];
	v1[1] = vertex[3 * triangles[3 * i + 1] + 1] - vertex[3 * triangles[3 * i + 0] + 1];
	v1[2] = vertex[3 * triangles[3 * i + 1] + 2] - vertex[3 * triangles[3 * i + 0] + 2];
	v2[0] = vertex[3 * triangles[3 * i + 2] + 0] - vertex[3 * triangles[3 * i + 1] + 0];
	v2[1] = vertex[3 * triangles[3 * i + 2] + 1] - vertex[3 * triangles[3 * i + 1] + 1];
	v2[2] = vertex[3 * triangles[3 * i + 2] + 2] - vertex[3 * triangles[3 * i + 1] + 2];
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v1[2] * v2[0] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void DrawPyramid()
{
	for (int i = 0; i < TrianglesNumber; i++)
	{
		GLfloat n[3];
		Normal(n, i);
		glNormal3fv(n);
		glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
		glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
		glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
	}
}

void Inside(GLint A, GLint B, GLenum cull_face, GLenum stencil_func)
{
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(cull_face);
	glCallList(A);
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);
	glCallList(B);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);
	glCallList(B);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(stencil_func, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glCullFace(cull_face);
	glCallList(A);
	glDisable(GL_STENCIL_TEST);
}

void DisplayScene()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -(near + far) / 2);
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_CULL_FACE);
	glCallList(option);
	glFlush();
	glutSwapBuffers();
}

void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (aspect == ASPECT_1_1)
	{
		if (width < height && width > 0)
		{
			glFrustum(left, right, bottom * height / width, top * height / width, near, far);
		}
		else
		{
			if (width >= height && height > 0)
			{
				glFrustum(left * width / height, right * width / height, bottom, top, near, far);
			}
		}
	}
	else
	{
		glFrustum(left, right, bottom, top, near, far);
	}

	DisplayScene();
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		button_state = state;

		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void Menu(int value)
{
	switch (value)
	{
	case SPHERE:
	{
		option = Sphere;
		DisplayScene();
	}
	break;

	case TORUS:
	{
		option = Torus;
		DisplayScene();
	}
	break;

	case TORUS_AND_SPHERE:
	{
		option = TorusANDSphere;
		DisplayScene();
	}
	break;

	case PYRAMID:
	{
		option = Pyramid;
		DisplayScene();
	}
	break;

	case PYRADMID_SUB_SPHERE:
	{
		option = PyramidSubSphere;
		DisplayScene();
	}
	break;

	case OR:
	{
		option = Or;
		DisplayScene();
	}
	break;

	case FULL_WINDOW:
	{
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	break;

	case ASPECT_1_1:
	{
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	break;

	case EXIT:
		exit(0);
	}
}

void GenerateDisplayLists()
{
	Torus = glGenLists(1);
	glNewList(Torus, GL_COMPILE);
	glColor4fv(Red);
	glutSolidTorus(0.3, 0.5, 30, 30);
	glEndList();
	Sphere = glGenLists(1);
	glNewList(Sphere, GL_COMPILE);
	glColor4fv(Green);
	glutSolidSphere(0.5, 30, 30);
	glEndList();
	TorusANDSphere = glGenLists(1);
	glNewList(TorusANDSphere, GL_COMPILE);
	Inside(Torus, Sphere, GL_BACK, GL_NOTEQUAL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_ALWAYS);
	glCallList(Torus);
	glDepthFunc(GL_LESS);
	Inside(Torus, Sphere, GL_BACK, GL_NOTEQUAL);
	glEndList();
	GenerateVerticles(vertex, N);
	GenerateTriangles(triangles, N);
	Pyramid = glGenLists(1);
	glNewList(Pyramid, GL_COMPILE);
	glColor4fv(Red);
	glBegin(GL_TRIANGLES);
	DrawPyramid();
	glEnd();
	glEndList();
	PyramidSubSphere = glGenLists(1);
	glNewList(PyramidSubSphere, GL_COMPILE);
	Inside(Pyramid, Sphere, GL_FRONT, GL_NOTEQUAL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_ALWAYS);
	glCallList(Sphere);
	glDepthFunc(GL_LESS);
	Inside(Sphere, Pyramid, GL_BACK, GL_EQUAL);
	glEndList();
	Or = glGenLists(1);
	glNewList(Or, GL_COMPILE);
	glEnable(GL_DEPTH_TEST);
	glCallList(PyramidSubSphere);
	glCallList(TorusANDSphere);
	glDisable(GL_DEPTH_TEST);
	glEndList();
	option = Or;
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(500, 500);
	glutCreateWindow("CSG");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	int MenuCSGOp = glutCreateMenu(Menu);
	glutAddMenuEntry("Sfera", SPHERE);
	glutAddMenuEntry("Ostroslup", PYRAMID);
	glutAddMenuEntry("Torus", TORUS);
	glutAddMenuEntry("Ostroslup SUB Kula", PYRADMID_SUB_SPHERE);
	glutAddMenuEntry("Torus AND Sphere", TORUS_AND_SPHERE);
	glutAddMenuEntry("A OR B", OR);
	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - ca�e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutCreateMenu(Menu);
	glutAddSubMenu("CSG", MenuCSGOp);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyj�cie", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	GenerateDisplayLists();
	glutMainLoop();
	return 0;
}