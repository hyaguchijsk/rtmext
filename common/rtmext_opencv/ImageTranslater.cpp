/**
   @file ImageTranslater.cpp
   @brief Image Translater, OpenCV cv::Mat <-> CameraImage
   @author Hiroaki Yaguchi, JSK
*/

#include "ImageTranslater.h"

using namespace std;
using namespace RTC;
using namespace cv;

bool convertToCV(CameraImage &rtmimage,Mat &cvimage){
  int type;

  switch(rtmimage.bpp){
  case 0:
    return decodeImageToCV(rtmimage,cvimage);
    break;
  case 8:
    type=CV_8UC1;
    break;
  case 11: // Kinect Depth
    return convertKinectDepthToCV(rtmimage,cvimage);
    break;
  case 24:
    type=CV_8UC3;
    break;
  default:
    cerr << "unconvertible bpp: " << rtmimage.bpp << endl;
    return false;
  }

  if(cvimage.cols!=rtmimage.width || cvimage.rows!=rtmimage.height || cvimage.type()!=type){
    cvimage.create(rtmimage.height,rtmimage.width,type);
  }

  unsigned char *buf=cvimage.ptr();
  if(type==CV_8UC3){
    for(int i=0;i<cvimage.cols*cvimage.rows*3;i+=3){
      buf[i  ]=rtmimage.pixels[i+2];
      buf[i+1]=rtmimage.pixels[i+1];
      buf[i+2]=rtmimage.pixels[i  ];
    }
  }else if(type==CV_8UC1){
    for(int i=0;i<cvimage.cols*cvimage.rows;i++)
      buf[i]=rtmimage.pixels[i];
  }
  return true;
}
bool convertToRTM(Mat &cvimage,CameraImage &rtmimage){
  int depth;

  switch(cvimage.type()){
  case CV_8UC1:
    rtmimage.bpp=8;
    depth=1;
    break;
  case CV_8UC3:
    rtmimage.bpp=24;
    depth=3;
    break;
  default:
    cerr << "unconvertible type: " << cvimage.type() << endl;
    return false;
  }

  rtmimage.width=cvimage.cols;
  rtmimage.height=cvimage.rows;
  rtmimage.format="RGB";

  rtmimage.pixels.length(rtmimage.width*rtmimage.height*depth);

  const unsigned char *buf=cvimage.ptr();
  if(depth==3){
    for(int i=0;i<cvimage.cols*cvimage.rows*3;i+=3){
      rtmimage.pixels[i  ]=buf[i+2];
      rtmimage.pixels[i+1]=buf[i+1];
      rtmimage.pixels[i+2]=buf[i  ];
    }
  }else if(depth==3){
    for(int i=0;i<cvimage.cols*cvimage.rows;i++)
      rtmimage.pixels[i]=buf[i];
  }
  return true;
}

bool encodeImageToRTM(string type, Mat &cvimage, CameraImage &rtmimage){
  rtmimage.width=cvimage.cols;
  rtmimage.height=cvimage.rows;
  rtmimage.format=type.c_str();
  rtmimage.bpp=0;

  vector<unsigned char> buf;
  imencode(type,cvimage,buf);
  if(buf.size()==0) return false;

  rtmimage.pixels.length(buf.size());
  for(int i=0;i<buf.size();i++)
    rtmimage.pixels[i]=buf[i];

  return true;

}
bool decodeImageToCV(CameraImage &rtmimage, Mat &cvimage){
  if(rtmimage.bpp > 0)
    return false;

  vector<unsigned char> buf(rtmimage.pixels.length());
  for(int i=0;i<rtmimage.pixels.length();i++)
    buf[i]=rtmimage.pixels[i];
  Mat mat=imdecode(Mat(buf),CV_LOAD_IMAGE_COLOR);
  mat.copyTo(cvimage);

  return true;
}
bool convertKinectDepthToCV(CameraImage &rtmimage, Mat &cvimage){
  if(rtmimage.bpp != 11)
    return false;

  cvimage.create(rtmimage.height,rtmimage.width,CV_8UC3);
  Mat hsv(rtmimage.height,rtmimage.width,CV_8UC3);

  unsigned char *buf=hsv.ptr();
  for(int i=0;i<rtmimage.height*rtmimage.width;i++){
    int idx=i*3;
    int didx=i*2;
    unsigned short depth=rtmimage.pixels[didx]+((unsigned short)rtmimage.pixels[didx+1]<<8);
    if(depth<1024){
      float col=((float)depth-512)*120.0/512.0;
      col=(col>0.0)?col:0.0;
      col=(col<120.0)?col:150.0;
      buf[idx]=(unsigned char)col;
      buf[idx+1]=buf[idx+2]=255;
    }else{
      buf[idx]=buf[idx+1]=buf[idx+2]=0;
    }
  }
  cvtColor(hsv,cvimage,CV_HSV2BGR);

  return true;
}
