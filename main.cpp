#include <iostream>
#include <opencv2/opencv.hpp>
#include "Marker/MarkerIdentification.h"
#include "Marker/MarkerTracking.hpp"
#include "PoseEstimation/PoseEstimation.h"
#include "ComputerVision/ComputerVision.hpp"
#include <iomanip>

#include "DrawUnits.h"
#include "OpenGL/GLMethods.hpp"
#include "GameField/GameField.hpp"

#define GLFW_INCLUDE_GLU // add support for GLU with GLFW
#include <GLFW/glfw3.h> /// this also includes other openGL headers

#include <ctime>
#include <vector>
#include <chrono>
#include <thread>

using namespace cv;
using namespace std;

using namespace std::chrono;

void trackbarHandler(int pos, void* slider_value) {
	*((int*)slider_value) = pos;
}

int main(int ac, char** av)
{
	GameField gamefield = GameField();
    
    //Computer Vision Stuff
    //create heap on startup
    CvMemStorage* memStorage =cvCreateMemStorage();
    VideoCapture cap(0); // open the default camera
	// VideoCapture cap(1); // open the USB camera
    if(!cap.isOpened()) { // check if we succeeded
        std::cout << "No camera found!\n"; //In this case, we show the supplied video
    }
    
    namedWindow("frame",1);
	int xChange = 330;
	int yChange = 274;
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
    
	int i = 0;
    while (true) {
        Mat frame;
        cap >> frame; // get a new frame from camera
        
        Point2f gameBoardCorners[4];
        findGameBoardCorners(frame, gameBoardCorners, memStorage);
  
        cv::Point2f targetCorners[4];
        targetCorners[0].x = 0; targetCorners[0].y = 0;
		targetCorners[1].x = FIELD_SIZE_COLS; targetCorners[1].y = 0;
		targetCorners[2].x = FIELD_SIZE_COLS; targetCorners[2].y = FIELD_SIZE_ROWS;
		targetCorners[3].x = 0; targetCorners[3].y = FIELD_SIZE_ROWS;
        
        //3x3 Transformationsmatrix
        Mat projMat(cv::Size(3, 3), CV_32FC1);
        projMat = getPerspectiveTransform(gameBoardCorners, targetCorners);
        
        
		Mat grid(Size(FIELD_SIZE_COLS, FIELD_SIZE_ROWS), CV_32FC1);
        
        
        //change the perspective in the marker image using the previously calculated matrix
		warpPerspective(frame, grid, projMat, Size(FIELD_SIZE_COLS, FIELD_SIZE_ROWS));
        
        Mat grayGrid;
        cvtColor(grid, grayGrid, CV_BGR2GRAY);
       
        
        Mat thresholdedGrid;
        threshold(grayGrid, thresholdedGrid, 110, 255, CV_THRESH_BINARY); //applies thresholding to gray Image
        
        
        imshow("more swag", thresholdedGrid);
        
        cv::Matx33f warp = projMat;
        
        // Punkt relative zum Spielfeld (Koordinaten des Punkts entsprechen Koordinaten relativ zum Spielfeld)
		cv::Point2f relative_point = cv::Point2f(gamefield.getMonsterPosition().at(1), gamefield.getMonsterPosition().at(0));
        
        // Punkt mit der inversen projektiven Transformationsmatrix multiplizieren
        cv::Point3f homogeneos = warp.inv() * relative_point;
        
        // Ergebnispunkt -> nun relativ zum gesamten Kamerabild
        cv::Point2f result(homogeneos.x, homogeneos.y);
        
        circle(frame, result, 6, Scalar(255,255,255), -1);
        
        milliseconds startTime = duration_cast< milliseconds >(
                                                               system_clock::now().time_since_epoch()
                                                               );
        
        // check if monster has reached the exit. if yes, place it back on the start. if no, move it by one step
		if (gamefield.getMonsterPosition() == std::vector<int>({ FIELD_SIZE_ROWS / 2, FIELD_SIZE_COLS - 1 })){
            std::cout << "EXIT was reached by the monster. Monster will be placed at the start again.\n";
            // put monster on start field on gamefield
            gamefield.setMonsterPosition(vector<int>({
                FIELD_SIZE_ROWS / 2,
                0
            }));
        }
        else{
			gamefield.monsterStep();
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
                                    milliseconds(100) - milliseconds(elapsed)
                                    );
        
        // print out the current monster position
		std::cout << i << "\tMonster relative position: \tx: " << gamefield.getMonsterPosition().at(1) << "\ty: " << gamefield.getMonsterPosition().at(0) << "\n";
		std::cout << i << "\tMonster absolute position: \tx: " << homogeneos.x << "\ty: " << homogeneos.y << "\n";
        i++;
        
        // imshow("frame", frame);
		float resultMatrix[16];
		const double kMarkerSize = 0.028;

		Point2f figureCorners[4];
		/*figureCorners[0] = Point2f(homogeneous.x-50, homogeneous.y-50);
		figureCorners[1] = Point2f(homogeneous.x+50, homogeneous.y-50);
		figureCorners[2] = Point2f(homogeneous.x+50, homogeneous.y+50);
		figureCorners[3] = Point2f(homogeneous.x-50, homogeneous.y+50);*/
		/* cout << homogeneous.x - 550;
		cout << "\n"; */
		figureCorners[0] = Point2f(0.7*(homogeneos.x - xChange), homogeneos.y - yChange + 20);
		figureCorners[1] = Point2f(0.7*(homogeneos.x - xChange +20), homogeneos.y - yChange + 20);
		figureCorners[2] = Point2f(0.7*(homogeneos.x - xChange +20), homogeneos.y - yChange);
		figureCorners[3] = Point2f(0.7*(homogeneos.x - xChange), homogeneos.y - yChange);
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
