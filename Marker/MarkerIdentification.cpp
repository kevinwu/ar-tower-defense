#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "MarkerIdentification.h"

//Since a marker can have 4 possible identifiers due to 4 orientations, we will compute the smallest one
string generateMarkerIdentifier(Mat &rectified){
    string ids[4];
    for(int i = 1;i<=4;i++){
        int rowResults[4] = {0};
        
        for(int j = 1;j<=4;j++){
            //For first orientation
            rowResults[0] = rowResults[0] + pow(2, 4-j)*((int)rectified.at<uchar>(i,j)==0?1:0);
            //second orientation
            rowResults[1] = rowResults[1] + pow(2, 4-j)*((int)rectified.at<uchar>(j,5-i)==0?1:0);
            //third orientation
            rowResults[2] = rowResults[2] + pow(2, 4-j)*((int)rectified.at<uchar>(5-i,5-j)==0?1:0);
            //fourth orientation
            rowResults[3] = rowResults[3] + pow(2, 4-j)*((int)rectified.at<uchar>(5-j,i)==0?1:0);
        }
        
        ids[0].append(to_string((int)rowResults[0]));
        ids[1].append(to_string((int)rowResults[1]));
        ids[2].append(to_string((int)rowResults[2]));
        ids[3].append(to_string((int)rowResults[3]));
        
    }
    
    int identifiers[4];
    
    for(int i = 0;i<4;i++){
        identifiers[i] = stoi(ids[i]);
    }
    
    //Selection Sort
    for(int i = 0;i<4;i++){
        for(int j = i+1;j<4;j++){
            if(identifiers[i]>identifiers[j]){
                int tmp = identifiers[j];
                identifiers[j] = identifiers[i];
                identifiers[i] = tmp;
                
                string tmpUseful = ids[j];
                ids[j] = ids[i];
                ids[i] = tmpUseful;
            }
        }
    }
    return ids[0];
}

void initialiseMarkerRectangle(Point2f destinationRectangle[4]){
    destinationRectangle[0] = Point2f(5.5,5.5);
    destinationRectangle[1] = Point2f(-0.5,5.5);
    destinationRectangle[2] = Point2f(-0.5,-0.5);
    destinationRectangle[3] = Point2f(5.5,-0.5);
}

void magnifyAndShowMarker(Mat rectified){
    Mat magnified(Size(60,60), CV_8UC1);
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            for(int k=0;k<10;k++){
                for(int l=0;l<10;l++){
                    magnified.at<uchar>(10*i+k,10*j+l) = (int)rectified.at<uchar>(i,j);
                }
            }
        }
    }
    imshow("Marker", magnified);
}

bool checkForBlackBorder(Mat &rectified){
    bool hasBlackBorder = true;
    for(int i= 0;i<6;i++){
        for(int j = 0;j<6;j++){
            if(i==0||i==5||j==0||j==5){
                
                if((int)rectified.at<uchar>(i,j)!=0){
                    
                    hasBlackBorder = false;
                    break;
                }
            }
        }
    }
    return hasBlackBorder;
}

