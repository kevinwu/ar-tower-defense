
#define GLFW_INCLUDE_GLU
#include <glfw/glfw3.h>
//#include <GL/gl.h>

#include <math.h>


/* PI */
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


void drawCube(float height, float width) {

	glBegin(GL_POLYGON);
	glVertex3f(width, -height, width);
	glVertex3f(width, height, width);
	glVertex3f(-width, height, width);
	glVertex3f(-width, -height, width);
	glEnd();

	// Purple side - RIGHT
	glBegin(GL_POLYGON);
	glVertex3f(width, -height, -width);
	glVertex3f(width, height, -width);
	glVertex3f(width, height, width);
	glVertex3f(width, -height, width);
	glEnd();

	// Green side - LEFT
	glBegin(GL_POLYGON);
	glVertex3f(-width, -height, width);
	glVertex3f(-width, height, width);
	glVertex3f(-width, height, -width);
	glVertex3f(-width, -height, -width);
	glEnd();

	// Blue side - TOP
	glBegin(GL_POLYGON);
	glVertex3f(width, height, width);
	glVertex3f(width, height, -width);
	glVertex3f(-width, height, -width);
	glVertex3f(-width, height, width);
	glEnd();

	// Red side - BOTTOM
	glBegin(GL_POLYGON);
	glVertex3f(width, -height, -width);
	glVertex3f(width, -height, width);
	glVertex3f(-width, -height, width);
	glVertex3f(-width, -height, -width);
	glEnd();
}

void drawPyramid(float pyramidHeight, float width)
{
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, pyramidHeight, 0.0f);
	glVertex3f(-width, -pyramidHeight, width);
	glVertex3f(width, -pyramidHeight, width);

	glVertex3f(0.0f, pyramidHeight, 0.0f);
	glVertex3f(-width, -pyramidHeight, -width);
	glVertex3f(width, -pyramidHeight, -width);

	glVertex3f(0.0f, pyramidHeight, 0.0f);
	glVertex3f(-width, -pyramidHeight, width);
	glVertex3f(-width, -pyramidHeight, -width);

	glVertex3f(0.0f, pyramidHeight, 0.0f);
	glVertex3f(width, -pyramidHeight, width);
	glVertex3f(width, -pyramidHeight, -width);

	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(-width, -pyramidHeight, width);
	glVertex3f(-width, -pyramidHeight, -width);
	glVertex3f(width, -pyramidHeight, -width);
	glVertex3f(width, -pyramidHeight, width);

	glEnd();

}


void drawCylinder(float height) {
	gluCylinder(gluNewQuadric(), 0.4, 0.4, height, 10, 1);
}

void drawEllipsoid(unsigned int uiStacks, unsigned int uiSlices, float fA, float fB, float fC)
{
	float tStep = (M_PI) / (float)uiSlices;
	float sStep = (M_PI) / (float)uiStacks;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (float t = -(M_PI) / 2; t <= (M_PI / 2) + .0001; t += tStep)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (float s = -(M_PI); s <= M_PI + .0001; s += sStep)
		{
			glVertex3f(fA * cos(t) * cos(s), fB * cos(t) * sin(s), fC * sin(t));
			glVertex3f(fA * cos(t + tStep) * cos(s), fB * cos(t + tStep) * sin(s), fC * sin(t + tStep));
		}
		glEnd();
	}
}

void drawSphere(double r, int lats, int longs) {
	int i, j;
	for (i = 0; i <= lats; i++) {
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = r * sin(lat0);
		double zr0 = r *  cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = r * sin(lat1);
		double zr1 = r * cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) {
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}
}


void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);

	gluCylinder(quadric, base, (GLdouble)0.0, height, slices, stacks);
	const GLdouble innerradius = 0.0;
	gluDisk(quadric, innerradius, base, slices, stacks);

	gluDeleteQuadric(quadric);
}


void drawMannequin(float enemyHeight) {

	float width = enemyHeight / 2;
	float depth = enemyHeight / 3;

	glPushMatrix();

	//legs
	glTranslatef(((width / 2) - (width*0.2)), 0.0, 0.0);
	glColor4f(0.1, 0.4, 0.2, 1.0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(gluNewQuadric(), width / 3, width / 2, enemyHeight / 2, 10, 1);

	glTranslatef(-(width - (width * 0.4)), 0, 0);
	gluCylinder(gluNewQuadric(), width / 3, width / 2, enemyHeight / 2, 10, 1);
	glPopMatrix();

	//body
	float bodyHeight = enemyHeight / 2.3;
	glTranslatef(0, enemyHeight*0.7, 0);
	glColor4f(0.6, 0.45, 0.45, 1.0);
	drawEllipsoid(10, 10, width, bodyHeight, depth + (depth*0.1));
	glPushMatrix();


	//head
	float headsize = width / 3;
	glTranslatef(0, bodyHeight*1.05, 0);
	glColor4f(0.6, 0.6, 0.6, 1.0);
	drawSphere(headsize, 8, 8);

	glPopMatrix();

	glPushMatrix();
	//arms
	float armLength = width / 3;
	glTranslatef(width*0.9, bodyHeight / 4, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(-20, 1, 0, 0);
	gluCylinder(gluNewQuadric(), width / 10, width / 10, armLength, 10, 1);

	glPopMatrix();

	glTranslatef(-((width*0.9) + armLength), bodyHeight / 4 + (width / 10), 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(20, 1, 0, 0);
	gluCylinder(gluNewQuadric(), width / 10, width / 10, armLength, 10, 1);


	//sword
	float swordLength = armLength * 2;
	float swordWidth = width / 10;
	//glTranslatef(-swordLength / 2, 0, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -(width / 10));
	glColor4f(0.7, 0.7, 0.7, 1.0);
	gluDisk(gluNewQuadric(), swordWidth, swordWidth * 2, 6, 1);
	glTranslatef(0, 0, -swordLength / 1.5);
	drawEllipsoid(8, 8, swordWidth, swordWidth / 2, swordLength);


}


void drawTurret(float turretHeight) {

	float cubeHeight = turretHeight;
	float pyramidHeight = turretHeight / 2;
	float width = turretHeight*0.6;
	float cylinderHeight = turretHeight;

	glTranslatef(0, turretHeight, 0);

	glColor4f(0.3, 0.3, 0.3, 1.0);
	drawCube(cubeHeight, width);

	glTranslatef(0, cubeHeight + pyramidHeight, 0);
	glColor4f(0.4, 0.16, 0.16, 1.0);
	drawPyramid(pyramidHeight, width*1.5);

	glTranslatef(0, (pyramidHeight / 10), (width / 3));
	glRotatef(-20, 1.0, 0.0, 0.0);
	glColor4f(0.7, 0.8, 0.8, 1.0);
	drawCylinder(cylinderHeight);

	/*
	//render sphere
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawSphere(1.0, 20, 20); //Radius und Anzahl der Kanten
	//move local coordinate system
	glTranslatef(0.0, 1.0, 0.0);
	//render sphere
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawSphere(0.7, 20, 20);
	//move local coordinate system
	glTranslatef(0.0, 0.7, 0.0);
	//render sphere
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawSphere(0.5, 20, 20);
	//move local coordinate system
	glTranslatef(0.0, 0.1, 0.2);
	//render nose
	glColor4f(1.0, 0.5, 0.0, 1.0);
	drawCone(0.3, 1, 8, 2);
	//move local coordinate system
	glTranslatef(0.25, 0.3, 0.3);
	//render eyes
	glColor4f(0.2, 0.1, 0.0, 1.0);
	drawSphere(0.1, 6, 6);
	//move local coordinate system
	glTranslatef(-0.5, 0.0, 0.0);
	//render eyes
	glColor4f(0.2, 0.1, 0.0, 1.0);
	drawSphere(0.1, 6, 6);
	*/
}