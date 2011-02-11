/**
   @file ImageTranslater.h
   @brief Image Translater, OpenCV cv::Mat <-> CameraImage
   @author Hiroaki Yaguchi, JSK
*/

#ifndef __IMAGETRANSLATER_H__
#define __IMAGETRANSLATER_H__

#include <iostream>
#include <vector>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

using namespace std;
using namespace RTC;
using namespace cv;

bool convertToCV(CameraImage &rtmimage,Mat &cvimage);
bool convertToRTM(Mat &cvimage,CameraImage &rtmimage);

bool encodeImageToRTM(string type, Mat &cvimage, CameraImage &rtmimage);
bool decodeImageToCV(CameraImage &rtmimage, Mat &cvimage);

#endif
