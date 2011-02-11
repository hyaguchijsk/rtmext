/**
   @file Coordinate.h
   @breif Coordinate class
   @author Hiroaki Yaguchi, JSK
 */

#ifndef __COORDINATE_H__
#define __COORDINATE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <string>

#include <rtm/idl/BasicDataTypeSkel.h>
#include "TimedCoordinateSkel.h"

using namespace std;
using namespace RTC;

class Coordinate{
 private:
  float pos[3],rot[9];
  string parent_id,frame_id;
  unsigned long sec,nsec;

 public:
  Coordinate();
  Coordinate(string &frame);
  Coordinate(string &parent,string &frame);
  Coordinate(float *newpos,float *rmat);
  Coordinate(vector <float> &newpos,vector <float> &rmat);
  Coordinate(string &frame,float *newpos,float *rmat);
  Coordinate(string &frame,vector <float> &newpos,vector <float> &rmat);
  Coordinate(string &parent,string &frame,float *newpos,float *rmat);
  Coordinate(string &parent,string &frame,vector <float> &newpos,vector <float> &rmat);
  Coordinate(Coordinate &obj);

  ~Coordinate(){}

  // setter / getter

  void setIdentity();

  void setFrameID(string &id){frame_id=id;}
  string getFrameID(){return frame_id;}
  void setParentID(string &id){parent_id=id;}
  string getParentID(){return parent_id;}

  void setTime(unsigned long s,unsigned long ns){sec=s; nsec=ns;}
  unsigned long getSecond(){return sec;}
  unsigned long getNanoSecond(){return nsec;}

  void setCoordinate(float *newpos,float *rmat);
  void setCoordinate(vector <float> &newpos,vector <float> &rmat);
  void setCoordinate(Coordinate &co);
  void getCoordinate(float *newpos,float *rmat);
  void getCoordinate(vector <float> &newpos,vector <float> &rmat);
  void getCoordinate(Coordinate &co);

  void set4x4(float *mat);
  void set4x4(vector<float> &mat);
  void get4x4(float *mat);
  void get4x4(vector <float> &mat);

  void setTranslation(float *newpos);
  void setTranslation(vector <float> &newpos);
  void getTranslation(float *newpos);
  void getTranslation(vector <float> &newpos);

  void setRotationMatrix(float *rmat);
  void setRotationMatrix(vector <float> &rmat);
  void getRotationMatrix(float *rmat);
  void getRotationMatrix(vector <float> &rmat);

  void setRotationQuaternion(float *quat);
  void setRotationQuaternion(vector<float> &quat);
  void getRotationQuaternion(float *quat);
  void getRotationQuaternion(vector<float> &quat);

  // for RTC
  void fromTimedCoordinate(TimedCoordinate &rtc_co);
  void toTimedCoordinate(TimedCoordinate &rtc_co);

  // coordinate operations
  void transform(Coordinate &co);
  void transform(float *trans,float *rmat);
  void transform(vector<float> &trans,vector<float> &rmat);
  void transform(float *tf);
  void transform(vector<float> &tf);
  void inverseCoordinate(Coordinate &co);
  Coordinate inverseCoordinate();

  // vector operations
  void rotateVector(float *vec);
  void rotateVector(vector <float> &vec);
  void transformVector(float *vec);
  void transformVector(vector <float> &vec);

};


#endif
