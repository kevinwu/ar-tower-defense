#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Video
{
public:
	Video();
	Mat getFrame();
private:
	VideoCapture cap;
};

Video::Video()
{
	Video::cap = VideoCapture(0); // open the default camera
	// Video::cap = VideoCapture(1); // open the default camera

	if (!cap.isOpened()) { // check if we succeeded
		cout << "No camera found!\n"; //In this case, we show the supplied video
	}
}

Mat Video::getFrame()
{
	Mat frame;
	cap >> frame; // get a new frame from camera
	return frame;
}