#include <iostream>
#include <opencv2/opencv.hpp>
#include "MarkerIdentification.h"
#include "MarkerTracking.hpp"
#include "PoseEstimation.h"
#include "ComputerVision.hpp"
#include <iomanip>
#include <glfw/glfw3.h>

#include <ctime>
#include <vector>
#include <chrono>
#include <thread>

using namespace cv;
using namespace std;


#define FIELD_SIZE_ROWS 11
#define FIELD_SIZE_COLS 11

const int EXIT = -1;
const int FREE_FIELD = 0;

using namespace std::chrono;

std::vector<int> getNextMonsterStep(std::vector<int> monster_current_gamefield_position, int gamefield_matrix[][FIELD_SIZE_COLS])
{
    // for the beginning, just move the monster by x+1.
    // FUTUREWORK: More sophisticated path finding
    return std::vector<int>({
        monster_current_gamefield_position.at(0),
        monster_current_gamefield_position.at(1) + 1
    });
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
    VideoCapture cap(1); // open the default camera
    if(!cap.isOpened()) { // check if we succeeded
        std::cout << "No camera found!\n"; //In this case, we show the supplied video
    }
    
    namedWindow("frame",1);
    
    while (true) {
        Mat frame;
        cap >> frame; // get a new frame from camera
        
        Point2f gameBoardCorners[4];
        cout << gameBoardCorners[0];
        findGameBoardCorners(frame, gameBoardCorners, memStorage);
        cout << gameBoardCorners[0];
        cv::Point2f targetCorners[4];
        targetCorners[0].x = 0; targetCorners[0].y = 0;
        targetCorners[1].x = 480; targetCorners[1].y = 0;
        targetCorners[2].x = 480; targetCorners[2].y = 480;
        targetCorners[3].x = 0; targetCorners[3].y = 480;
        
        //3x3 Transformationsmatrix
        Mat projMat(cv::Size(3, 3), CV_32FC1);
        projMat = getPerspectiveTransform(gameBoardCorners, targetCorners);
        
        
        Mat grid(Size(480, 480), CV_32FC1);
        
        
        //change the perspective in the marker image using the previously calculated matrix
        warpPerspective(frame, grid, projMat, Size(480, 480));
        
        Mat grayGrid;
        cvtColor(grid, grayGrid, CV_BGR2GRAY);
        imshow("gray", grayGrid);
        
        Mat thresholdedGrid;
        threshold(grayGrid, thresholdedGrid, 110, 255, CV_THRESH_BINARY); //applies thresholding to gray Image
        
        imshow("frame", grid);
        imshow("more swag", thresholdedGrid);
        
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
                                    milliseconds(1000) - milliseconds(elapsed)
                                    );
        
        // print out the current monster position
        std::cout << i << "\tMonster position: \tx: " << monster_current_gamefield_position.at(1) << "\ty: " << monster_current_gamefield_position.at(0) << "\n";
        i++;
        
        imshow("frame", frame);
        char keypress;
        keypress = waitKey(30);
        if(keypress==27) break;
        
        //Reinitialise heap- at end of processing loop
        cvClearMemStorage(memStorage);
    }
    
    //release heap (program exit)
    cvReleaseMemStorage(&memStorage);
    
}
