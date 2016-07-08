#define GLFW_INCLUDE_GLU
#include <glfw/glfw3.h>

//camera settings
const int camera_width = 640;
const int camera_height = 480;
const int virtual_camera_angle = 30;
unsigned char bkgnd[camera_width*camera_height * 3];

/* program & OpenGL initialization */
void initGL(int argc, char *argv[])
{
	// initialize the GL library

	// Added in Exercise 8 - End *****************************************************************
	// pixel storage/packing stuff
	glPixelStorei(GL_PACK_ALIGNMENT, 1); // for glReadPixels?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // for glTexImage2D?
	glPixelZoom(1.0, -1.0);
	// Added in Exercise 8 - End *****************************************************************

	// enable and set colors
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0, 0, 0, 1.0);

	// enable and set depth parameters
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// light parameters
	GLfloat light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_amb[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_dif[] = { 0.7, 0.7, 0.7, 1.0 };

	// enable lighting
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


}

void display(GLFWwindow* window, const cv::Mat &img_bgr, float resultMatrix[16])
{
	// Added in Exercise 8 - Start *****************************************************************
	memcpy(bkgnd, img_bgr.data, sizeof(bkgnd));
	// Added in Exercise 8 - End *****************************************************************

	int width0, height0;
	glfwGetFramebufferSize(window, &width0, &height0);
	//	reshape(window, width, height);

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Added in Exercise 8 - Start *****************************************************************
	// draw background image
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, camera_width, 0.0, camera_height);

	glRasterPos2i(0, camera_height - 1);
	glDrawPixels(camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd);

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	// Added in Exercise 8 - End *****************************************************************

	// move to marker-position
	glMatrixMode(GL_MODELVIEW);

	float resultTransposedMatrix[16];
	for (int x = 0; x<4; ++x)
	{
		for (int y = 0; y<4; ++y)
		{
			resultTransposedMatrix[x * 4 + y] = resultMatrix[y * 4 + x];
		}
	}


	// CHECK OPENGL COORDINATE SYSTEM: http://www.cocos2d-x.org/attachments/download/1563




	//glLoadTransposeMatrixf( resultMatrix );
	glLoadMatrixf(resultTransposedMatrix);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.03, 0.03, 0.03);

	glTranslatef(0, -5, 0);


	// draw 3 white spheres
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawSphere(0.8, 10, 10);
	glTranslatef(0.0, 0.8, 0.0);
	drawSphere(0.6, 10, 10);
	glTranslatef(0.0, 0.6, 0.0);
	drawSphere(0.4, 10, 10);

	// draw the eyes
	glPushMatrix();
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glTranslatef(0.2, 0.2, 0.2);
	drawSphere(0.066, 10, 10);
	glTranslatef(0, 0, -0.4);
	drawSphere(0.066, 10, 10);
	glPopMatrix();

	// draw a nose
	glColor4f(1.0, 0.5, 0.0, 1.0);
	glTranslatef(0.3, 0.0, 0.0);
	glRotatef(90, 0, 1, 0);
	drawCone(0.1, 0.3, 10, 10);


}

void reshape(GLFWwindow* window, int width, int height) {

	// set a whole-window viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// create a perspective projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Note: Just setting the Perspective is an easy hack. In fact, the camera should be calibrated.
	// With such a calibration we would get the projection matrix. This matrix could then be loaded 
	// to GL_PROJECTION.
	// If you are using another camera (which you'll do in most cases), you'll have to adjust the FOV
	// value. How? Fiddle around: Move Marker to edge of display and check if you have to increase or 
	// decrease.
	gluPerspective(virtual_camera_angle, ((GLfloat)width / (GLfloat)height), 0.01, 100);

	// invalidate display
	//glutPostRedisplay();
}