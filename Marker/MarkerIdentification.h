#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//Get the Marker ID based on Marker
string generateMarkerIdentifier(Mat &rectified);

//Check if matrix has a black border
bool checkForBlackBorder(Mat &rectified);

void magnifyAndShowMarker(Mat rectified);

void initialiseMarkerRectangle(Point2f destinationRectangle[4]);
