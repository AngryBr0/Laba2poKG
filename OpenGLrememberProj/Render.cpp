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
using namespace std;
bool textureMode = true;
bool lightMode = true;

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



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

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

void create_edges(double h)
{
	double A[] = { 0,0,h };
	double B[] = { 0,4,h };
	double C[] = { 5,5,h };

	double D[] = { 4, 0,h };

	double E[] = { 10,-3,h };
	double F[] = { 3,-3,h };

	double G[] = { 1,-7,h};

	double K[] = { -5,-1,h };


	glBegin(GL_TRIANGLES);

	glColor3d(0.2, 0.6, 0.9);
	glNormal3f(0, 0, h > 0 ? 1 : -1);

	glTexCoord2d(0, 0.47);
	glVertex3dv(A);
	glTexCoord2d(0, 0.73);
	glVertex3dv(B);
	glTexCoord2d(0.27, 0.47);
	glVertex3dv(D);

	glNormal3f(0, 0, h > 0 ? 1 : -1);
	glTexCoord2d(0.5 * 2, 0.2 * 2);
	glVertex3dv(D);
	glTexCoord2d(0.1 * 2, 0.8 * 2);
	glVertex3dv(E);
	glTexCoord2d(0.7 * 2, 0.6 * 2);
	glVertex3dv(F);

	glNormal3f(0, 0, h > 0 ? 1 : -1);
	glTexCoord2d(0, 2);
	glVertex3dv(A);
	glTexCoord2d(2, 1);
	glVertex3dv(D);
	glTexCoord2d(1, 2);
	glVertex3dv(F);


	glEnd();
}

float normal[] = { 0, 0, 0 };

void calc_vector(vector <float> a, vector <float> b) {
	normal[0] = (a[1] * b[2] - b[1] * a[2]);
	normal[1] =  (-a[0] * b[2] + b[0] * a[2])  ;
	normal[2] = a[0] * b[1] - b[0] * a[1];
}

void create_side()
{
	glBegin(GL_QUADS);

	glColor3d(0.2, 0.6, 0.9);

	vector <float> vtr_1 = { 0,4,0 };
	vector <float> vtr_2 = { 0,0,-1 };
	calc_vector(vtr_1, vtr_2);
	glNormal3d(normal[0], normal[1], normal[2]);

	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glVertex3d(0, 4, 1);
	glVertex3d(0, 4, 0);

	vtr_1 = { 1 ,5, 0 };
	vtr_2 = { 0, 0, 1 };
	calc_vector(vtr_1, vtr_2);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3d(5, 5, 0);
	glVertex3d(5, 5, 1);
	glVertex3d(4, 0, 1);
	glVertex3d(4, 0, 0);

	vtr_1 = { 6,-3,0 };
	vtr_2 = { 0,0,-1 };
	calc_vector(vtr_1, vtr_2);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(4, 0, 0);
	glVertex3d(4, 0, 1);
	glVertex3d(10, -3, 1);
	glVertex3d(10, -3, 0);

	vtr_1 = { -7,0,0 };
	vtr_2 = { 0,0,-1 };
	calc_vector(vtr_1, vtr_2);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(10, -3, 1);
	glVertex3d(10, -3, 0);
	glVertex3d(3, -3, 0);
	glVertex3d(3, -3, 1);

	vtr_1 = { -2,-4,0 };
	vtr_2 = { 0,0,-1 };
	calc_vector(vtr_1, vtr_2);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(3, -3, 0);
	glVertex3d(3, -3, 1);
	glVertex3d(1, -7, 1);
	glVertex3d(1, -7, 0);

	glColor3d(0.2, 0.6, 0.9);
	vtr_1 = { 5,1,0 };
	vtr_2 = { 0,0,-1 };
	calc_vector(vtr_1, vtr_2);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(-5, -1, 0);
	glVertex3d(-5, -1, 1);
	glVertex3d(0, 0, 1);
	glVertex3d(0, 0, 0);

	glEnd();
}

/*
* �������, ������� ������������ ��������� ����� � �������������� ������ ������������ �����
*. ��� ������� ����������� ����.
*
* ���������:
* x (GLfloat) - ��������� x ����������� ����� �����
* y (GLfloat) - ��������� ����������� ����� ���������� �� ��� y
* radius (GLfloat) - ������, ������� ����� ����� ������������ ����
*/
void drawFilledCircle() {
	float radius = 3.6;
	float x = 0;
	float y = 0;
	int i;
	int triangleAmount = 40; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	glColor3d(0.2, 0.6, 0.9);
	glNormal3f(0, 0, -1);
	GLfloat twicePi = 2.0f * 3.141592f;
	glPushMatrix();
	glTranslated(3, 2, 0);
	glRotated(56.3, 0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(-1.11, -1.94, 0); // center of circle
	for (i = 0; i <= triangleAmount; i++) {
		double delta = i * twicePi / triangleAmount;
		if (delta <= 1.7)
		{
			glVertex3f(
				x + (radius * cos(i * twicePi / triangleAmount)),
				y + (radius * sin(i * twicePi / triangleAmount)), 0
			);
		}
	}
	glEnd();


	glPopMatrix();

	glPushMatrix();

	glTranslated(3, 2, 0);
	glRotated(56.3, 0, 0, 1);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0,  1);
	glVertex3f(-1.11, -1.94, 1);  // center of circle
	for (i = 0; i <= triangleAmount; i++) {
		double delta = i * twicePi / triangleAmount;
		if (delta <= 1.7)
		{
			glVertex3f(
				x + (radius * cos(i * twicePi / triangleAmount)),
				y + (radius * sin(i * twicePi / triangleAmount)), 1
			);
		}
	}
	glEnd();
	glPopMatrix();
}

void circle_side() {
	float radius = 3.6;
	float x = 0;
	float z = 0;
	int i;
	int triangleAmount = 40; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	glColor3f(0.0f, 1.0f, 0.0f);
	GLfloat twicePi = 2.0f * 3.141592f;
	glPushMatrix();
	glTranslated(3, 2, 1);
	glRotated(56.3, 0, 0, 1);
	glRotated(-90, 1, 0, 0);
	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= triangleAmount; i++) {
		double delta = i * twicePi / triangleAmount;
		if (delta <= 1.7)
		{
			glNormal3f(x + (radius * cos(i * twicePi / triangleAmount)), 0, z + (radius * sin(i * twicePi / triangleAmount)));
			glVertex3f(
				x + (radius * cos(i * twicePi / triangleAmount)), 0,
				z + (radius * sin(i * twicePi / triangleAmount))
			);
			glNormal3f(x + (radius * cos(i * twicePi / triangleAmount)), 0, z + (radius * sin(i * twicePi / triangleAmount)));
			glVertex3f(
				x + (radius * cos(i * twicePi / triangleAmount)), 1,
				z + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	}
	glEnd();
	glPopMatrix();
}

void concavity() {
	float radius = 10;
	float x = 0;
	float y = 0;
	int i;
	int triangleAmount = 1000; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	glColor3d(0.2, 0.6, 0.9);
	glNormal3f(0, 0, -1);
	GLfloat twicePi = 2.0f * 3.141592f;
	glPushMatrix();
	glTranslated(-8.4, -10.4, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(8.4f, 10.4, 0); // center of circle
	for (i = 0; i <= triangleAmount / 5; i++) {
		float delta = i * twicePi / triangleAmount;
		x = radius * cos(i * twicePi / triangleAmount);
		y = radius * sin(i * twicePi / triangleAmount);
		if (x <= 9.41f && y <= 9.41f)
		{
			glVertex3f(x, y, 0);
		}
	}
	glEnd();
	glPopMatrix();

	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0, -1);

	glVertex3d(0, 0, 0);
	glVertex3d(1, -7, 0);
	glVertex3d(3, -3, 0);

	glEnd();

	glPushMatrix();
	glTranslated(-8.4, -10.4, 1);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, 1);
	glVertex3f(8.4f, 10.4, 0); // center of circle
	for (i = 0; i <= triangleAmount / 5; i++) {
		float delta = i * twicePi / triangleAmount;
		x = radius * cos(i * twicePi / triangleAmount);
		y = radius * sin(i * twicePi / triangleAmount);
		if (x <= 9.41f && y <= 9.41f)
		{
			glVertex3f(x, y, 0);
		}
	}
	glEnd();
	glPopMatrix();

	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0,1);

	glVertex3d(0, 0, 1);
	glVertex3d(1, -7, 1);
	glVertex3d(3, -3, 1);

	glEnd();
}
void cancavity_side() {
	float radius = 10;
	float x = 0;
	float z = 0;
	int i;
	int triangleAmount = 800; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	glColor3f(0.3f, 0.4f, 0.6f);
	GLfloat twicePi = 2.0f * 3.141592f;
	glPushMatrix();
	//glRotated(-1, 0, 0, 1);
	glTranslated(-8.41, -10.4, 1);
	glRotated(-90, 1, 0, 0);
	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= triangleAmount; i++) {
		double delta = i * twicePi / triangleAmount;
		if (delta <= 1.23 && delta >= 0.34)
		{
			glNormal3f(-(x + (radius * cos(i * twicePi / triangleAmount))), 0, -(z + (radius * sin(i * twicePi / triangleAmount))));
			glVertex3f(
				x + (radius * cos(i * twicePi / triangleAmount)), 0,
				z + (radius * sin(i * twicePi / triangleAmount))
			);
			glNormal3f(-(x + (radius * cos(i * twicePi / triangleAmount))), 0, -(z + (radius * sin(i * twicePi / triangleAmount))));
			glVertex3f(
				x + (radius * cos(i * twicePi / triangleAmount)), 1,
				z + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	}
	glEnd();
	glPopMatrix();
}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


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


	//������ ��������� ���������� ��������

	create_edges(0);

	create_edges(1);

	create_side();

	drawFilledCircle();

	circle_side();

	concavity();

	cancavity_side();
	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);





	glEnd();
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
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
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