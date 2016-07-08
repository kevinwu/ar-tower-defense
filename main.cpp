#include <iostream>
#include <opencv2/opencv.hpp>
#include "Marker/MarkerIdentification.h"
#include "Marker/MarkerTracking.hpp"
#include "PoseEstimation/PoseEstimation.h"
#include "ComputerVision/ComputerVision.hpp"
#include <iomanip>

#include "DrawUnits.h"

#define GLFW_INCLUDE_GLU // add support for GLU with GLFW
#include <GLFW/glfw3.h> /// this also includes other openGL headers

#include <ctime>
#include <vector>
#include <chrono>
#include <thread>

using namespace cv;
using namespace std;


#define FIELD_SIZE_ROWS 20
#define FIELD_SIZE_COLS 20

const int EXIT = -1;
const int FREE_FIELD = 0;

//camera settings
const int camera_width = 640;
const int camera_height = 480;
const int virtual_camera_angle = 30;
unsigned char bkgnd[camera_width*camera_height * 3];

using namespace std::chrono;

void trackbarHandler(int pos, void* slider_value) {
	*((int*)slider_value) = pos;
}

std::vector<int> getNextMonsterStep(std::vector<int> monster_current_gamefield_position, int gamefield_matrix[][FIELD_SIZE_COLS])
{
    // for the beginning, just move the monster by x+1.
    // FUTUREWORK: More sophisticated path finding
    return std::vector<int>({
        monster_current_gamefield_position.at(0),
        monster_current_gamefield_position.at(1) + 1
    });
}

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


int main(int ac, char** av)
{
    // y, x
    std::vector<int> monster_current_gamefield_position = {
        FIELD_SIZE_ROWS / 2,
        -1
    };
    int gamefield_matrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS] = { FREE_FIELD }; //y,x (with the top left corner being (0,0)
    gamefield_matrix[FIELD_SIZE_ROWS / 2][FIELD_SIZE_COLS - 1] = EXIT;
    int i = 0;
    
    //Computer Vision Stuff
    //create heap on startup
    CvMemStorage* memStorage =cvCreateMemStorage();
    VideoCapture cap(0); // open the default camera
	// VideoCapture cap(1); // open the USB camera
    if(!cap.isOpened()) { // check if we succeeded
        std::cout << "No camera found!\n"; //In this case, we show the supplied video
    }
    
    namedWindow("frame",1);
	int xChange = 550;
	int yChange = 220;
	cv::createTrackbar("xChange", "frame", &xChange, 1024, trackbarHandler, &xChange);
	cv::createTrackbar("yChange", "frame", &yChange, 1024, trackbarHandler, &yChange);


	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	// initialize the window system
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(camera_width, camera_height, "Exercise 8 - Combine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set callback functions for GLFW
	glfwSetFramebufferSizeCallback(window, reshape);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	int window_width, window_height;
	glfwGetFramebufferSize(window, &window_width, &window_height);
	reshape(window, window_width, window_height);

	glViewport(0, 0, window_width, window_height);

	// initialize the GL library
	initGL(ac, av);
    
    while (true) {
        Mat frame;
        cap >> frame; // get a new frame from camera
        
        Point2f gameBoardCorners[4];
 
        findGameBoardCorners(frame, gameBoardCorners, memStorage);
  
        cv::Point2f targetCorners[4];
        targetCorners[0].x = 0; targetCorners[0].y = 0;
        targetCorners[1].x = 20; targetCorners[1].y = 0;
        targetCorners[2].x = 20; targetCorners[2].y = 20;
        targetCorners[3].x = 0; targetCorners[3].y = 20;
        
        //3x3 Transformationsmatrix
        Mat projMat(cv::Size(3, 3), CV_32FC1);
        projMat = getPerspectiveTransform(gameBoardCorners, targetCorners);
        
        
        Mat grid(Size(20, 20), CV_32FC1);
        
        
        //change the perspective in the marker image using the previously calculated matrix
        warpPerspective(frame, grid, projMat, Size(20, 20));
        
        Mat grayGrid;
        cvtColor(grid, grayGrid, CV_BGR2GRAY);
       
        
        Mat thresholdedGrid;
        threshold(grayGrid, thresholdedGrid, 110, 255, CV_THRESH_BINARY); //applies thresholding to gray Image
        
        
        imshow("more swag", thresholdedGrid);
        
        cv::Matx33f warp = projMat;
        
        // Punkt relative zum Spielfeld (Koordinaten des Punkts entsprechen Koordinaten relativ zum Spielfeld)
        cv::Point2f relative_point = cv::Point2f(monster_current_gamefield_position.at(1), monster_current_gamefield_position.at(0));
        
        // Punkt mit der inversen projektiven Transformationsmatrix multiplizieren
        cv::Point3f homogeneos = warp.inv() * relative_point;
        
        // Ergebnispunkt -> nun relativ zum gesamten Kamerabild
        cv::Point2f result(homogeneos.x, homogeneos.y);
        
        circle(frame, result, 6, Scalar(255,255,255), -1);
        
        milliseconds startTime = duration_cast< milliseconds >(
                                                               system_clock::now().time_since_epoch()
                                                               );
        
        // check if monster has reached the exit. if yes, place it back on the start. if no, move it by one step
        if (monster_current_gamefield_position == std::vector<int>({ FIELD_SIZE_ROWS / 2, FIELD_SIZE_COLS - 1 })){
            std::cout << "EXIT was reached by the monster. Monster will be placed at the start again.\n";
            // put monster on start field on gamefield
            monster_current_gamefield_position = {
                FIELD_SIZE_ROWS / 2,
                0
            };
        }
        else{
            monster_current_gamefield_position = getNextMonsterStep(monster_current_gamefield_position, gamefield_matrix);
        }
        
        // TODO: INTEGRATE: render gamefield with monster on it
        // 1. Find gamefield corners on the camera image
        // 2. Get projective tranformation matrix for transformation between camera image and game field
        // 3. Transform the relative coordinate of the monster into the aboslute coordinates on the map
        // 4. Render the monster on the absolute coordinate
        // FUTUREWORK: make an animated transition from the last absolute step coords to the next absolute step coords within 5s (continously get the current camera image, transformation matrix and render the monster with a minimal position change)
        // After the monster has reached the next step (after the 5s), we would continue the outer loop -> the monster does the next step (which itself would be animated to take 5s again)
        
        // Sleep until 1s have past from the beginning of this iteration.
        // Why? to maintain a constant step rate
        milliseconds endTime = duration_cast< milliseconds >(
                                                             system_clock::now().time_since_epoch()
                                                             );
        milliseconds elapsed = endTime - startTime;
        std::this_thread::sleep_for(
                                    milliseconds(200) - milliseconds(elapsed)
                                    );
        
        // print out the current monster position
        std::cout << i << "\tMonster position: \tx: " << monster_current_gamefield_position.at(1) << "\ty: " << monster_current_gamefield_position.at(0) << "\n";
        i++;
        
        // imshow("frame", frame);
		float resultMatrix[16];
		const double kMarkerSize = 0.048;

		Point2f figureCorners[4];
		/*figureCorners[0] = Point2f(homogeneous.x-50, homogeneous.y-50);
		figureCorners[1] = Point2f(homogeneous.x+50, homogeneous.y-50);
		figureCorners[2] = Point2f(homogeneous.x+50, homogeneous.y+50);
		figureCorners[3] = Point2f(homogeneous.x-50, homogeneous.y+50);*/
		/* cout << homogeneous.x - 550;
		cout << "\n"; */
		figureCorners[0] = Point2f(homogeneos.x - xChange, homogeneos.y - yChange + 10);
		figureCorners[1] = Point2f(homogeneos.x - xChange + 10, homogeneos.y - yChange + 10);
		figureCorners[2] = Point2f(homogeneos.x - xChange + 10, homogeneos.y - yChange);
		figureCorners[3] = Point2f(homogeneos.x - xChange, homogeneos.y - yChange);
		estimateSquarePose(resultMatrix, (cv::Point2f*)figureCorners, kMarkerSize); // TODO: shouldn't be gameBoardCorners, but the corners around the figure

		/* Render here */
		display(window, frame, resultMatrix);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

        char keypress;
        keypress = waitKey(30);
        if(keypress==27) break;
        
        //Reinitialise heap- at end of processing loop
        cvClearMemStorage(memStorage);
    }
    
    //release heap (program exit)
	glfwTerminate();
    cvReleaseMemStorage(&memStorage);
    
}
