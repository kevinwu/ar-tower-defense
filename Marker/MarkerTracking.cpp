#include "MarkerTracking.hpp"
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

//Basically, how to get subpixel intensity value
int subpixSampleSafe (const Mat &pSrc, const Point2f &p)
{
    int x = int( floorf ( p.x ) );
    int y = int( floorf ( p.y ) );
    if(x<0||x>=pSrc.cols -1|| y < 0 || y >= pSrc.rows - 1 )
        return 127;
    int dx = int ( 256 * ( p.x - floorf ( p.x ) ) );
    int dy = int ( 256 * ( p.y - floorf ( p.y ) ) );
    unsigned char* i = ( unsigned char* ) ( ( pSrc.data + y * pSrc.step ) + x );
    int a = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
    i += pSrc.step;
    int b = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
    return a + ( ( dy * ( b - a) ) >> 8 );
}

// Finds the intersection of two lines, or returns false.
// The lines are defined by (o1, p1) and (o2, p2).
bool lineIntersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2,
                      Point2f &r)
{
    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;
    
    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;
    
    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

void drawCornerPoints(Mat frame, Point2f cornerPoints[4]){
    for(int i=0;i<4;i++){
        circle(frame, cornerPoints[i], 6, Scalar(0,255,255), -1);
    }
}

void findPreciseCornerPoints(Point2f cornerPoints[4], Mat lineParamsMat){
    //At this point we have four lines saved in the rows of lineParamsMat
    //We intersect these to get the four corners
    for(int i = 0;i<4;i++){
        
        Point2f line1point1(lineParamsMat.at<float>(2,i), lineParamsMat.at<float>(3,i));
        Point2f line1point2(lineParamsMat.at<float>(2,i)+30*lineParamsMat.at<float>(0,i), lineParamsMat.at<float>(3,i)+30*lineParamsMat.at<float>(1,i));
        
        Point2f line2point1(lineParamsMat.at<float>(2,(i+1)%4), lineParamsMat.at<float>(3,(i+1)%4));
        Point2f line2point2(lineParamsMat.at<float>(2,(i+1)%4)+30*lineParamsMat.at<float>(0,(i+1)%4), lineParamsMat.at<float>(3,(i+1)%4)+30*lineParamsMat.at<float>(1,(i+1)%4));
        
        Point2f lineIntersect;
        
        
        lineIntersection(line1point1,line1point2,line2point1,line2point2, lineIntersect);
        cornerPoints[i] = lineIntersect;
        
    }
}

Point2f calculatePreciseEdgePoint(int stripeLength, Mat iplStripe, Point p, Point2f stripeVecY){
    //use sobel operator on stripe
    // ( -1 , -2, -1 )
    // (  0 ,  0,  0 )
    // (  1 ,  2,  1 )
    std::vector<double> sobelValues(stripeLength-2);
    ///					double* sobelValues = new double[stripeLength-2];
    for (int n = 1; n < (stripeLength-1); n++)
    {
        unsigned char* stripePtr = &(iplStripe.at<uchar>(n-1,0) );
        ///						unsigned char* stripePtr = ( unsigned char* )( iplStripe->imageData + (n-1) * iplStripe->widthStep );
        double r1 = -stripePtr[ 0 ] - 2 * stripePtr[ 1 ] - stripePtr[ 2 ];
        
        stripePtr += 2*iplStripe.step;
        ///						stripePtr += 2*iplStripe->widthStep;
        double r3 =  stripePtr[ 0 ] + 2 * stripePtr[ 1 ] + stripePtr[ 2 ];
        sobelValues[n-1] = r1+r3;
    }
    
    double maxVal = -1;
    int maxIndex = 0;
    for (int n=0; n<stripeLength-2; ++n)
    {
        if ( sobelValues[n] > maxVal )
        {
            maxVal = sobelValues[n];
            maxIndex = n;
        }
    }
    
    double y0,y1,y2; // y0 .. y1 .. y2
    y0 = (maxIndex <= 0) ? 0 : sobelValues[maxIndex-1];
    y1 = sobelValues[maxIndex];
    y2 = (maxIndex >= stripeLength-3) ? 0 : sobelValues[maxIndex+1];
    
    //formula for calculating the x-coordinate of the vertex of a parabola, given 3 points with equal distances
    //(xv means the x value of the vertex, d the distance between the points):
    //xv = x1 + (d / 2) * (y2 - y0)/(2*y1 - y0 - y2)
    
    double pos = (y2 - y0) / (4*y1 - 2*y0 - 2*y2 ); //d = 1 because of the normalization and x1 will be added later
    
    Point2f edgeCenter; //exact point with subpixel accuracy
    int maxIndexShift = maxIndex - (stripeLength>>1);
    
    //shift the original edgepoint accordingly
    edgeCenter.x = (double)p.x + (((double)maxIndexShift+pos) * stripeVecY.x);
    edgeCenter.y = (double)p.y + (((double)maxIndexShift+pos) * stripeVecY.y);
    
    return edgeCenter;
}

