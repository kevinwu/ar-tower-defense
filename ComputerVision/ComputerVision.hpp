//
//  ComputerVision.hpp
//  ARTowerDefense
//
//  Created by Kevin Wu on 07/07/16.
//  Copyright © 2016 Kevin Wu. All rights reserved.
//

#ifndef ComputerVision_hpp
#define ComputerVision_hpp

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "MarkerIdentification.h"
#include "MarkerTracking.hpp"
#include "PoseEstimation.h"

#endif /* ComputerVision_hpp */

using namespace cv;
using namespace std;

void findGameBoardCorners (Mat &frame, Point2f gameBoardCorners[4], CvMemStorage* memStorage);
