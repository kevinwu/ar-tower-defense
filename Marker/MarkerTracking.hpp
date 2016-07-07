#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//Basically, how to get subpixel intensity value
int subpixSampleSafe (const Mat &pSrc, const Point2f &p);

//Get 2 lines defined by two points and find intersection
bool lineIntersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);

//Using two lines, find the intersection
void findPreciseCornerPoints(Point2f cornerPoints[4], Mat lineParamsMat);

//Given the lines surrounding a marker, find the middle point
Point2f findMarkerMiddlePoint(Mat lineParamsMat);

bool findTwoImplicitCorners(Point2f marker1Point, Mat lineParamsMarker1, Point2f marker2Point, Mat lineParamsMarker2, Point2f implicitCorners[2]);

//Given some corner points- render them onto the image
void drawCornerPoints(Mat frame, Point2f cornerPoints[4]);

//Use Sobel Operator to calculate precise edge point
Point2f calculatePreciseEdgePoint(int stripeLength, Mat iplStripe, Point p, Point2f stripeVecY);
