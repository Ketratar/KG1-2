#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


#define TOP_RIGHT 1.0f,1.0f
#define TOP_LEFT 0.0f,1.0f
#define BOTTOM_RIGHT 1.0f,0.0f
#define BOTTOM_LEFT 0.0f,0.0f


bool textureMode = true;
bool lightMode = true;
bool changeTextureMode = false;
bool alpha = false;


//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'Q')
	{
		changeTextureMode = !changeTextureMode;
	}

	if (key == 'A')
	{
		alpha = !alpha;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId[2];

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);


	//���������� �� ��� ��������
	glGenTextures(1, &texId[0]);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[0]);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	OpenGL::LoadBMP("texture2.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//���������� �� ��� ��������
	glGenTextures(1, &texId[1]);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[1]);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}



// ��� ����� ��� ����������, ������ ���� ���������� 0..1
double coord_for_x(double x)
{
	double new_x;
	new_x = x / 9;
	return new_x;
}

double coord_for_y(double y)
{
	double new_y;
	new_y = y / 6.08975;
	return new_y;
}
//------------------------------------------------------

void normali(double aa[], double bb[], double cc[], double dd[])
{
	//counter
	int i = 0;
	// initialize all vectors for walls if need for other points i'll inverse the vector
	double AB[3] = { 0 };
	double BC[3] = { 0 };
	double CD[3] = { 0 };
	double DA[3] = { 0 };

	//normal vectors
	double nx;
	double ny;
	double nz;

	//lenght of normal vector
	double len;

	//Calculate all vectors
	for (i = 0; i < 3; i++)
	{
		AB[i] = bb[i] - aa[i];
		BC[i] = cc[i] - bb[i];
		CD[i] = dd[i] - cc[i];
		DA[i] = aa[i] - dd[i];
	}

	//time for calculate normal-> vectors
	//for A (BA(a) x DA(b))

	nx = (-AB[1]) * DA[2] - DA[1] * (-AB[2]);
	ny = -(-AB[0]) * DA[2] + DA[0] * (-AB[2]);
	nz = (-AB[0]) * DA[1] - DA[0] * (-AB[1]);

	// calculate len
	len = sqrt(pow(nx, 2) + pow(ny, 2) + pow(nz, 2));

	glNormal3d(nx/len, ny/len, nz/len);

}

void stena1()
{
	double a[] = { 2.5, 2.5, 0 };
	double b[] = { 2.5, 2.5, 0.5 };
	double c[] = { 5.5, 5.5, 0.5 };
	double d[] = { 5.5, 5.5, 0 };
	
	normali(a, b, c, d);

	glBegin(GL_QUADS);
	
	glColor3d(0.75, 0.32, 0.32);
	
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(d);
	
	glEnd();

}

void stena2()
{
	double a[] = { 5.5, 5.5, 0 };
	double b[] = { 5.5, 5.5, 0.5 };
	double c[] = { 5,4,0.5 };
	double d[] = { 5,4,0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(a);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(b);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(c);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(d);

	glEnd();
}

void stena3()
{
	double a[] = { 5,4,0 };
	double b[] = { 5,4,0.5 };
	double c[] = { 7,5,0.5 };
	double d[] = { 7,5,0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	//glVertex3dv(a);
	//glVertex3dv(b);
	//glVertex3dv(c);
	//glVertex3dv(d);

	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(a);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(b);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(c);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(d);

	glEnd();
}

void stena4()
{
	double a[] = { 7,5,0 };
	double b[] = { 7,5,0.5 };
	double c[] = { 7,3,0.5 };
	double d[] = { 7,3,0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	//glVertex3dv(a);
	//glVertex3dv(b);
	//glVertex3dv(c);
	//glVertex3dv(d);

	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(a);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(b);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(c);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(d);

	glEnd();
}

void stena5()
{
	double a[] = { 7,3,0 };
	double b[] = { 7,3,0.5 };
	double c[] = { 9,3,0.5 };
	double d[] = { 9,3,0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	//glVertex3dv(a);
	//glVertex3dv(b);
	//glVertex3dv(c);
	//glVertex3dv(d);

	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(a);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(b);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(c);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(d);

	glEnd();
}

void stena6()
{
	double a[] = { 9,3,0 };
	double b[] = { 9,3,0.5 };
	double c[] = { 6, 2, 0.5 };
	double d[] = { 6, 2, 0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	//glVertex3dv(a);
	//glVertex3dv(b);
	//glVertex3dv(c);
	//glVertex3dv(d);

	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(a);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(b);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(c);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(d);

	glEnd();
}

void stena7()
{
	double a[] = { 6,2,0 };
	double b[] = { 6,2,0.5 };
	double c[] = { 5,0,0.5 };
	double d[] = { 5,0,0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	//glVertex3dv(a);
	//glVertex3dv(b);
	//glVertex3dv(c);
	//glVertex3dv(d);

	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(a);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(b);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(c);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(d);

	glEnd();
}

void stena8()
{
	double a[] = { 5,0,0 };
	double b[] = { 5,0,0.5 };
	double c[] = { 2.5, 2.5, 0.5 };
	double d[] = { 2.5, 2.5, 0 };

	normali(a, b, c, d);

	glBegin(GL_QUADS);

	glColor3d(0.75, 0.32, 0.32);

	//glVertex3dv(a);
	//glVertex3dv(b);
	//glVertex3dv(c);
	//glVertex3dv(d);

	glEnd();
}

void polotok()
{
	double a[] = { 2.5, 2.5, 0.5 };
	double b[] = { 5.5, 5.5, 0.5 };
	double c[] = { 5,4,0.5 };
	double d[] = { 7,5,0.5 };
	double e[] = { 7,3,0.5 };
	double f[] = { 9,3,0.5 };
	double g[] = { 6, 2, 0.5 };
	double h[] = { 5,0,0.5 };
	double i[] = { 2.5, 2.5, 0.5 };

	int it = 0;
	const double points = 100;
	double r = 2.12;
	double shatie_a = 1.2; //1.2
	double shatie_b = 4;
	const double cx = 3.74;
	const double cy = 1.25;
	const double pi = 3.14;
	double angle0 = -0.78;


	//for potolok C

	const double cx_c = 4.0303;
	const double cy_c = 3.97;
	double angle0_c = 0.8017;

	//-------------------------------------------------------


	//��������� ����� ���������

	if (alpha)
	{
		glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE);
	}

	//----------------------------
#pragma region ����������_�������

	glColor3d(0.75, 0.75, 0.32);

	glNormal3d(0, 0, 1);

	glBegin(GL_POLYGON);

	for (it = 0; it < points; it++)
	{
		const double angle = ((pi * it) / points);
		glTexCoord2d(coord_for_x(cx_c + r * cos(angle + angle0_c)), coord_for_y(cy_c + r * sin(angle + angle0_c)));
		glVertex3d(cx_c + r * cos(angle + angle0_c), cy_c + r * sin(angle + angle0_c), 0.5);
	}
	glEnd();

#pragma endregion ����������_�������

	glNormal3d(0, 0, 1);

	glBegin(GL_POLYGON_BIT);

	glColor3d(0.75, 0.75, 0.35);

	for (it = 0; it < points; it++)
	{
		glTexCoord2d(coord_for_x(d[0]), coord_for_y(d[1]));
		glVertex3dv(d);
		const double angle = ((pi * it) / points);
		glTexCoord2d(coord_for_x(((r * cos(angle) / shatie_a) * cos(angle0)) - ((r * sin(angle) / shatie_b) * sin(angle0)) + cx), coord_for_y(((r * cos(angle) / shatie_a) * sin(angle0)) + ((r * sin(angle) / shatie_b) * cos(angle0)) + cy));
		glVertex3d(((r * cos(angle) / shatie_a) * cos(angle0)) - ((r * sin(angle) / shatie_b) * sin(angle0)) + cx, ((r * cos(angle) / shatie_a) * sin(angle0)) + ((r * sin(angle) / shatie_b) * cos(angle0)) + cy, 0.5);
	}
	glEnd();

	glNormal3d(0, 0, 1);

	glBegin(GL_POLYGON);

	glColor3d(0.75, 0.75, 0.35);

	glTexCoord2d(coord_for_x(a[0]), coord_for_y(a[1]));
	glVertex3dv(a);
	glTexCoord2d(coord_for_x(b[0]), coord_for_y(b[1]));
	glVertex3dv(b);
	glTexCoord2d(coord_for_x(c[0]), coord_for_y(c[1]));
	glVertex3dv(c);
	glTexCoord2d(coord_for_x(d[0]), coord_for_y(d[1]));
	glVertex3dv(d);

	glEnd();

	glNormal3d(0, 0, 1);

	glBegin(GL_POLYGON);

	glColor3d(0.75, 0.75, 0.35);

	glTexCoord2d(coord_for_x(g[0]), coord_for_y(g[1]));
	glVertex3dv(g);
	glTexCoord2d(coord_for_x(h[0]), coord_for_y(h[1]));
	glVertex3dv(h);
	glTexCoord2d(coord_for_x(d[0]), coord_for_y(d[1]));
	glVertex3dv(d);
	glTexCoord2d(coord_for_x(e[0]), coord_for_y(e[1]));
	glVertex3dv(e);
	glTexCoord2d(coord_for_x(f[0]), coord_for_y(f[1]));
	glVertex3dv(f);

	glEnd();
}

void pol()
{
	double a[] = { 2.5, 2.5, 0 };
	double b[] = { 5.5, 5.5, 0 };
	double c[] = { 5,4,0 };
	double d[] = { 7,5,0 };
	double e[] = { 7,3,0 };
	double f[] = { 9,3,0 };
	double g[] = { 6, 2, 0 };
	double h[] = { 5,0,0 };
	double i[] = { 2.5, 2.5, 0 };

	int it = 0;
	const double points = 100;
	double r = 2.12;
	double shatie_a = 1.2;
	double shatie_b = 4;
	const double cx = 3.74;
	const double cy = 1.25;
	const double pi = 3.14;
	double angle0 = -0.78;

	normali(a, b, c, d);

	glBegin(GL_POLYGON_BIT);
	glColor3d(0.2, 0.75, 0.6);

	for (it = 0; it < points; it++)
	{
		glTexCoord2d(coord_for_x(d[0]), coord_for_y(d[1]));
		glVertex3dv(d);
		const double angle = ((pi * it) / points);
		glTexCoord2d(coord_for_x(((r * cos(angle) / shatie_a) * cos(angle0)) - ((r * sin(angle) / shatie_b) * sin(angle0)) + cx), coord_for_y(((r * cos(angle) / shatie_a) * sin(angle0)) + ((r * sin(angle) / shatie_b) * cos(angle0)) + cy));
		glVertex3d(((r * cos(angle) / shatie_a) * cos(angle0)) - ((r * sin(angle) / shatie_b) * sin(angle0)) + cx, ((r * cos(angle) / shatie_a) * sin(angle0)) + ((r * sin(angle) / shatie_b) * cos(angle0)) + cy, 0);
	}
	glEnd();

	glBegin(GL_POLYGON);

	glColor3d(0.2, 0.75, 0.6);

	glTexCoord2d(coord_for_x(a[0]), coord_for_y(a[1]));
	glVertex3dv(a);
	glTexCoord2d(coord_for_x(b[0]), coord_for_y(b[1]));
	glVertex3dv(b);
	glTexCoord2d(coord_for_x(c[0]), coord_for_y(c[1]));
	glVertex3dv(c);
	glTexCoord2d(coord_for_x(d[0]), coord_for_y(d[1]));
	glVertex3dv(d);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3d(0.2, 0.75, 0.6);

	glTexCoord2d(coord_for_x(g[0]), coord_for_y(g[1]));
	glVertex3dv(g);
	glTexCoord2d(coord_for_x(h[0]), coord_for_y(h[1]));
	glVertex3dv(h);
	glTexCoord2d(coord_for_x(d[0]), coord_for_y(d[1]));
	glVertex3dv(d);
	glTexCoord2d(coord_for_x(e[0]), coord_for_y(e[1]));
	glVertex3dv(e);
	glTexCoord2d(coord_for_x(f[0]), coord_for_y(f[1]));
	glVertex3dv(f);

	glEnd();

}

void _C()
{
	int i = 0;
	const double points = 100;
	double r = 2.12;
	const double cx = 4.0303;
	const double cy = 3.97;
	const double pi = 3.14;
	double angle0 = 0.8017;

	double a[3];
	double b[3];
	double c[3];
	double d[3];

	a[2] = 0.5;
	b[2] = 0;
	c[2] = 0.5;
	d[2] = 0;

#pragma region ����������_���

	glColor3d(0.2, 0.75, 0.6);
	glNormal3d(0, 0, -1);

	glBegin(GL_POLYGON);

	for (i = 0; i < points; i++)
	{
		const double angle = ((pi * i) / points);
		glTexCoord2d(coord_for_x(cx + r * cos(angle + angle0)), coord_for_y(cy + r * sin(angle + angle0)));
		glVertex2d(cx + r * cos(angle + angle0), cy + r * sin(angle + angle0));
	}

	glEnd();

#pragma endregion ����������_���


#pragma region ����������_�����
//
//	glColor3d(0.75, 0.32, 0.32);
//
//	glBegin(GL_QUAD_STRIP);
//
//	for (i = 0; i < points; i++)
//	{
//		const double angle = ((pi * i) / points);
//		glVertex3d(cx + r * cos(angle + angle0), cy + r * sin(angle + angle0), 0.5);
//		glVertex3d(cx + r * cos(angle + angle0), cy + r * sin(angle + angle0), 0);
//	}
//	glEnd();
//

	for (i = 0; i < points; i++)
	{
		const double angle = ((pi * i) / points);
	
		if (i % 2 != 0)
		{
			c[0] = cx + r * cos(angle + angle0);
			c[1] = cy + r * sin(angle + angle0);
	
			d[0] = c[0];
			d[1] = c[1];
	
			normali(a, b, a, c);
	
			glColor3d(0.75, 0.32, 0.32);
	
			glBegin(GL_QUADS);
			
			glTexCoord2d((i-1)/points, 1);
			glVertex3dv(a);
			glTexCoord2d((i)/points, 1);
			glVertex3dv(c);
			glTexCoord2d((i)/points, 0);
			glVertex3dv(d);
			glTexCoord2d((i-1)/points, 0);
			glVertex3dv(b);
	
			glEnd();
		}
		if (i % 2 == 0)
		{
			a[0] = cx + r * cos(angle + angle0);
			a[1] = cy + r * sin(angle + angle0);
	
			b[0] = a[0];
			b[1] = a[1];
			if (i > 1)
			{
				normali(c, b, c, a);
	
				glColor3d(0.75, 0.32, 0.32);
	
				glBegin(GL_QUADS);
	
				glTexCoord2d((i) / points, 1);
				glVertex3dv(a);
				glTexCoord2d((i-1) / points, 1);
				glVertex3dv(c);
				glTexCoord2d((i-1) / points, 0);
				glVertex3dv(d);
				glTexCoord2d((i) / points, 0);
				glVertex3dv(b);
	
				glEnd();
			}
		}
	}


#pragma endregion ����������_�����

}

void _C_OtherWay()
{
#pragma region ���������_�����

	//int i = 0;
	//const double points = 100;
	//double r = 2.12;
	//double a = 1.2; //1.2
	//double b = 4;
	//const double cx = 3.74;
	//const double cy = 1.25;
	//const double pi = 3.14;
	//double angle0 = -0.78;
	//
	//glColor3d(0.75, 0.32, 0.32);
	//
	//glBegin(GL_QUAD_STRIP);
	//
	//for (i = 0; i < points; i++)
	//{
	//	const double angle = ((pi * i) / points);
	//	glVertex3d(((r * cos(angle) / a) * cos(angle0)) - ((r * sin(angle) / b) * sin(angle0)) + cx, ((r * cos(angle) / a) * sin(angle0)) + ((r * sin(angle) / b) * cos(angle0)) + cy, 0);
	//	glVertex3d(((r * cos(angle) / a) * cos(angle0)) - ((r * sin(angle) / b) * sin(angle0)) + cx, ((r * cos(angle) / a) * sin(angle0)) + ((r * sin(angle) / b) * cos(angle0)) + cy, 0.5);
	//}
	//glEnd();

	int i = 0;
	const double points = 100;
	double r = 2.12;
	double a = 1.2; //1.2
	double b = 4;
	const double cx = 3.74;
	const double cy = 1.25;
	const double pi = 3.14;
	double angle0 = -0.78;

	double aa[3];
	double bb[3];
	double c[3];
	double d[3];

	aa[2] = 0.5;
	bb[2] = 0;
	c[2] = 0.5;
	d[2] = 0;

	for (i = 0; i < points; i++)
	{
		const double angle = ((pi * i) / points);

		if (i % 2 != 0)
		{
			c[0] = ((r * cos(angle) / a) * cos(angle0)) - ((r * sin(angle) / b) * sin(angle0)) + cx;
			c[1] = ((r * cos(angle) / a) * sin(angle0)) + ((r * sin(angle) / b) * cos(angle0)) + cy;

			d[0] = c[0];
			d[1] = c[1];

			normali(aa, c, aa, bb);

			glColor3d(0.75, 0.32, 0.32);

			glBegin(GL_QUADS);

			//glVertex3dv(aa);
			//glVertex3dv(bb);
			//glVertex3dv(d);
			//glVertex3dv(c);

			glTexCoord2d((i - 1) / points, 1);
			glVertex3dv(aa);
			glTexCoord2d((i) / points, 1);
			glVertex3dv(c);
			glTexCoord2d((i) / points, 0);
			glVertex3dv(d);
			glTexCoord2d((i - 1) / points, 0);
			glVertex3dv(bb);

			//glTexCoord2d(coord_for_x(aa[0]), coord_for_y(aa[1]));
			//glVertex3dv(aa);
			//glTexCoord2d(coord_for_x(bb[1]), coord_for_y(bb[0]));
			//glVertex3dv(bb);
			//glTexCoord2d(coord_for_x(c[0]), coord_for_y(c[1]));
			//glVertex3dv(c);
			//glTexCoord2d(coord_for_x(d[1]), coord_for_y(d[0]));
			//glVertex3dv(d);

			glEnd();
		}
		if (i % 2 == 0)
		{
			aa[0] = ((r * cos(angle) / a) * cos(angle0)) - ((r * sin(angle) / b) * sin(angle0)) + cx;
			aa[1] = ((r * cos(angle) / a) * sin(angle0)) + ((r * sin(angle) / b) * cos(angle0)) + cy;

			bb[0] = aa[0];
			bb[1] = aa[1];
			if (i > 1)
			{
				normali(c, aa, c, bb);

				glColor3d(0.75, 0.32, 0.32);

				glBegin(GL_QUADS);

				//glVertex3dv(aa);
				//glVertex3dv(bb);
				//glVertex3dv(d);
				//glVertex3dv(c);

				glTexCoord2d((i) / points, 1);
				glVertex3dv(aa);
				glTexCoord2d((i - 1) / points, 1);
				glVertex3dv(c);
				glTexCoord2d((i - 1) / points, 0);
				glVertex3dv(d);
				glTexCoord2d((i) / points, 0);
				glVertex3dv(bb);

				//glTexCoord2d(coord_for_x(c[0]), coord_for_y(c[1]));
				//glVertex3dv(c);
				//glTexCoord2d(coord_for_x(d[1]), coord_for_y(d[0]));
				//glVertex3dv(d);
				//glTexCoord2d(coord_for_x(aa[0]), coord_for_y(aa[1]));
				//glVertex3dv(aa);
				//glTexCoord2d(coord_for_x(bb[1]), coord_for_y(bb[0]));
				//glVertex3dv(bb);

				glEnd();
			}
		}
	}
#pragma endregion ���������_�����


}

void pol_plosko()
{
	double center[] = { 4.3, 2.7, 0 };
	double a[] = { 2.5, 2.5, 0 };
	double b[] = { 5.5, 5.5, 0 };
	double c[] = { 5,4,0 };
	double d[] = { 7,5,0 };
	double e[] = { 7,3,0 };
	double f[] = { 9,3,0 };
	double g[] = { 6, 2, 0 };
	double h[] = { 5,0,0 };
	double i[] = { 2.5, 2.5, 0 };

	normali(a, g, g, h);
	glBegin(GL_QUADS);

	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(c);

	glEnd();

	normali(d, c, b, b);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(c);
	glVertex3dv(d);
	glVertex3dv(e);

	glEnd();

	normali(e, c, c, d);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(e);
	glVertex3dv(f);
	glVertex3dv(g);

	glEnd();

	normali(g, e, e, f);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(g);
	glVertex3dv(h);
	glVertex3dv(i);

	glEnd();
}

void polotok_plosko()
{
	double center[] = {4.3, 2.7, 0.5};
	double a[] = { 2.5, 2.5, 0.5 };
	double b[] = { 5.5, 5.5, 0.5 };
	double c[] = { 5,4,0.5 };
	double d[] = { 7,5,0.5 };
	double e[] = { 7,3,0.5 };
	double f[] = { 9,3,0.5 };
	double g[] = { 6, 2, 0.5 };
	double h[] = { 5,0,0.5 };
	double i[] = { 2.5, 2.5, 0.5 };

	normali(a, h, g, g);
	glBegin(GL_QUADS);

	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(c);

	glEnd();

	normali(c, b, a, a);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(c);
	glVertex3dv(d);
	glVertex3dv(e);

	glEnd();

	normali(f, g, f, h);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(e);
	glVertex3dv(f);
	glVertex3dv(g);

	glEnd();

	normali(h, g, h, a);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.75, 0.6);

	glVertex3dv(center);
	glVertex3dv(g);
	glVertex3dv(h);
	glVertex3dv(i);

	glEnd();
}


void _Zadanie()
{
	_C();
	//stena1();
	stena2();
	stena3();
	stena4();
	stena5();
	stena6();
	stena7();
	//stena8();
	//pol_plosko();
	pol();
	//polotok_plosko();
	_C_OtherWay();
	polotok();
}


void Render(OpenGL *ogl)
{
	glBindTexture(GL_TEXTURE_2D, texId[0]);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	if (changeTextureMode)
		glBindTexture(GL_TEXTURE_2D, texId[1]);

	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  



	_Zadanie();
	

















	//������ ��������� ���������� ��������
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };
	//
	//glBindTexture(GL_TEXTURE_2D, texId);
	//
	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);
	//
	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);
	//
	//glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "Q - ����� �������" << std::endl;
	ss << "A - ���/���� �����-���������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}