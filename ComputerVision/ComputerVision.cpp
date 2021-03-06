//
//  ComputerVision.cpp
//  ARTowerDefense
//
//  Created by Kevin Wu on 07/07/16.
//  Copyright © 2016 Kevin Wu. All rights reserved.
//

#include "ComputerVision.hpp"

void findGameBoardCorners (Mat &frame, Point2f gameBoardCorners[4], CvMemStorage* memStorage){
    
    /// Initialize values
    int alpha_slider = 110;
    
    Mat grayFrame;
    cvtColor(frame, grayFrame, CV_BGR2GRAY); //first parameter is input image, second parameter output image
    
    
    Mat thresholded;
    
    threshold(grayFrame, thresholded, alpha_slider, 255, CV_THRESH_BINARY); //applies thresholding to gray Image
    imshow("Thresholded Camera Image", thresholded);
    CvSeq* contours;
    CvMat thresholded_(thresholded);
    
    cvFindContours(&thresholded_, memStorage, &contours);
    
    Point2f marker1Point;
    Point2f marker2Point;
    Mat lineParamsMarker1;
    Mat lineParamsMarker2;
    
    int numberSeenContours = 0;
    
    
    //traversing all contours
    for(; contours; contours = contours->h_next){
        
        
        
        CvSeq* result = cvApproxPoly(contours, sizeof(CvContour), memStorage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02);
        
        if(result->total!=4) continue;
        
        Mat result_ = cvarrToMat(result);
        Rect r = boundingRect(result_);
        
        //check for size
        if (r.height<30 || r.width<30 || r.height>150|| r.width>150|| r.width > thresholded.cols - 10 || r.height > thresholded.rows - 10) {
            continue;
        }
        
        const Point *rect = (const Point*) result_.data;
        int npts = result_.rows;
        
        polylines(frame, &rect, &npts, 1, true, Scalar(0,0,255),2, CV_AA, 0);
        
        //4 by 4 Matrix;
        Mat lineParamsMat(Size(4,4), CV_32F);
        
        //We are looking at one specific contour at this point
        for(int i = 0;i<4;i++){
            circle(frame, rect[i], 6, Scalar(0,255,0), -1);
            
            //We want to now create stripes that allow us to get the edge point with subpixel accuracy
            
            //distance between stripes or 1/7 of the distance between rectangle corners
            double dx = (double)(rect[(i+1)%4].x-rect[i].x)/7.0;
            double dy = (double)(rect[(i+1)%4].y-rect[i].y)/7.0;
            
            // Stripe size
            int stripeLength = (int)(0.8*sqrt (dx*dx+dy*dy));
            if (stripeLength < 5)
                stripeLength = 5;
            
            //make stripe length odd
            stripeLength |= 1;
            
            Size stripeSize;
            stripeSize.width = 3;
            stripeSize.height = stripeLength;
            
            // Direction vectors
            Point2f stripeVecX;
            Point2f stripeVecY;
            double diffLength = sqrt ( dx*dx+dy*dy );
            stripeVecX.x = dx / diffLength;
            stripeVecX.y = dy / diffLength;
            stripeVecY.x = stripeVecX.y;
            stripeVecY.y = -stripeVecX.x;
            
            Mat iplStripe(stripeSize, CV_8UC1);
            
            //Array for edge point centers
            Point2f edgePoints[6];
            
            for (int j=1; j<7; ++j)
            {
                double px = (double)rect[i].x+(double)j*dx;
                double py = (double)rect[i].y+(double)j*dy;
                
                Point p;
                p.x = (int)px;
                p.y = (int)py;
                
                cv::circle ( frame, p, 4, CV_RGB(0,0,255), -1);
                
                //Let's deal with Stripes
                Mat iplStripe( stripeSize, CV_8UC1 );
                
                //Iterate over Stripe Width
                for(int m = -1; m <=1; ++m) {
                    // Stripe length
                    for(int n = 0-(stripeLength/2); n <= stripeLength/2; ++n ) {
                        Point2f subPixel;
                        subPixel.x = (double)p.x + ((double)m * stripeVecX.x) + ((double)n * stripeVecY.x);
                        subPixel.y = (double)p.y + ((double)m * stripeVecX.y) + ((double)n * stripeVecY.y);
                        
                        int pixel = subpixSampleSafe(grayFrame, subPixel);
                        int w = m + 1;
                        int h = n + ( stripeLength >> 1);
                        //Define our stripe - pixel relations
                        iplStripe.at<uchar>(h,w) = (uchar)pixel;
                        circle (frame, subPixel, 2, CV_RGB(0,0,255), -1);
                        
                    }
                }
                edgePoints[j-1] = calculatePreciseEdgePoint(stripeLength, iplStripe, p, stripeVecY);
            }
            
            //point parameters for line
            Mat point_mat(Size(1, 6), CV_32FC2, edgePoints);
            
            //The line is stored in a column in the lineParamsMat Matrix
            fitLine(point_mat, lineParamsMat.col(i), CV_DIST_L2, 0, 0.01, 0.01);
        }
        
        Point2f cornerPoints[4];
        
        findPreciseCornerPoints(cornerPoints, lineParamsMat);
        
        if(numberSeenContours==0){
            marker1Point = cornerPoints[1];
            lineParamsMarker1 = lineParamsMat;
            
        } else{
            marker2Point = cornerPoints[2];
            lineParamsMarker2 = lineParamsMat;
            
        }
        
        if(++numberSeenContours==2){
            break;
        }
        
        
        
    }
    

    if(marker1Point.x>marker2Point.x){
        Point2f tmpPoint = marker1Point;
        marker1Point = marker2Point;
        marker2Point = tmpPoint;
        
        Mat tmpMat = lineParamsMarker1;
        lineParamsMarker1 = lineParamsMarker2;
        lineParamsMarker2 = tmpMat;
    }
        
    Point2f implicitCorners[2];
        
    findTwoImplicitCorners(marker1Point, lineParamsMarker1, marker2Point, lineParamsMarker2, implicitCorners);
    
    circle(frame, marker1Point, 6, Scalar(0,255,255), -1);
    circle(frame, marker2Point, 6, Scalar(0,255,255), -1);
    circle(frame, implicitCorners[0], 6, Scalar(0,255,255), -1);
    circle(frame, implicitCorners[1], 6, Scalar(0,255,255), -1);
    
    gameBoardCorners[0] = marker1Point;
    gameBoardCorners[3] = implicitCorners[1];
    gameBoardCorners[2] = marker2Point;
    gameBoardCorners[1] = implicitCorners[0];
}
