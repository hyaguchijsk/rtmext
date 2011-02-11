/**
   @file Coordinate.cpp
   @breif Coordinate class
   @author Hiroaki Yaguchi, JSK
 */

#include "Coordinate.h"

using namespace std;
using namespace RTC;

Coordinate::Coordinate(){
  setIdentity();
}
Coordinate::Coordinate(string &frame){
  setFrameID(frame);
  setIdentity();
}
Coordinate::Coordinate(string &parent,string &frame){
  setParentID(parent);
  setFrameID(frame);
  setIdentity();
}
Coordinate::Coordinate(float *newpos,float *rmat){
  setCoordinate(newpos,rmat);
}
Coordinate::Coordinate(vector <float> &newpos,vector <float> &rmat){
  setCoordinate(newpos,rmat);
}
Coordinate::Coordinate(string &frame,float *newpos,float *rmat){
  setFrameID(frame);
  setCoordinate(newpos,rmat);
}
Coordinate::Coordinate(string &frame,vector <float> &newpos,vector <float> &rmat){
  setFrameID(frame);
  setCoordinate(newpos,rmat);
}
Coordinate::Coordinate(string &parent,string &frame,float *newpos,float *rmat){
  setParentID(parent);
  setFrameID(frame);
  setCoordinate(newpos,rmat);
}
Coordinate::Coordinate(string &parent,string &frame,vector <float> &newpos,vector <float> &rmat){
  setParentID(parent);
  setFrameID(frame);
  setCoordinate(newpos,rmat);
}

/**
   @brief Copy Constructor
   @param obj reference object
 */
Coordinate::Coordinate(Coordinate &obj){
  setCoordinate(obj);
}

/**
   @brief Setting Identity Matrix
 */
void Coordinate::setIdentity(){
  pos[0]=pos[1]=pos[2]=0.0;
  rot[0]=rot[4]=rot[8]=1.0;
  rot[1]=rot[2]=rot[3]=rot[5]=rot[6]=rot[7]=0.0;
}

/**
   @brief Setter of position vector and rotation matrix
 */
void Coordinate::setCoordinate(float *newpos,float *rmat){
  setTranslation(newpos);
  setRotationMatrix(rmat);
}
void Coordinate::setCoordinate(vector <float> &newpos,vector <float> &rmat){
  //if(newpos.size()!=3){cerr << "[Coordinate::setCoordinate] missmatched format of position vector" << endl; return;}
  //if(rmat.size()!=3){cerr << "[Coordinate::setCoordinate] missmatched format of rotation matrix" << endl; return;}
  setCoordinate(&newpos[0],&rmat[0]);
}
void Coordinate::setCoordinate(Coordinate &co){
  co.getTranslation(pos);
  co.getRotationMatrix(rot);
  string pid=co.getParentID();
  string fid=co.getFrameID();
  setParentID(pid);
  setFrameID(fid);
  setTime(co.getSecond(),co.getNanoSecond());
}

/**
   @brief Getter of position vector and rotation matrix
 */
void Coordinate::getCoordinate(float *newpos,float *rmat){
  getTranslation(newpos);
  getRotationMatrix(rmat);
}
void Coordinate::getCoordinate(vector <float> &newpos,vector <float> &rmat){
  newpos.resize(3);
  rmat.resize(9);
  getCoordinate(&newpos[0],&rmat[0]);
}
void Coordinate::getCoordinate(Coordinate &co){
  co.setTranslation(pos);
  co.setRotationMatrix(rot);
  co.setParentID(parent_id);
  co.setFrameID(frame_id);
  co.setTime(getSecond(),getNanoSecond());
}

/**
   @brief Setter of 4x4 matrix
 */
void Coordinate::set4x4(float *mat){
  memcpy(&rot[0],&mat[0],sizeof(float)*3);
  memcpy(&rot[3],&mat[4],sizeof(float)*3);
  memcpy(&rot[6],&mat[8],sizeof(float)*3);
  pos[0]=mat[3];
  pos[1]=mat[7];
  pos[2]=mat[11];
}
void Coordinate::set4x4(vector<float> &mat){
  set4x4(&mat[0]);
}
/**
   @brief Getter of 4x4 matrix
 */
void Coordinate::get4x4(float *mat){
  memcpy(&mat[0],&rot[0],sizeof(float)*3);
  memcpy(&mat[4],&rot[3],sizeof(float)*3);
  memcpy(&mat[8],&rot[6],sizeof(float)*3);
  mat[3]=pos[0];
  mat[7]=pos[1];
  mat[11]=pos[2];
  mat[12]=mat[13]=mat[14]=0.0;
  mat[15]=1.0;
}
void Coordinate::get4x4(vector <float> &mat){
  mat.resize(16);
  get4x4(&mat[0]);
}

/**
   @brief Setter of translation vector
 */
void Coordinate::setTranslation(float *newpos){
  memcpy(pos,newpos,sizeof(float)*3);
}
void Coordinate::setTranslation(vector <float> &newpos){
  setTranslation(&newpos[0]);
}
/**
   @brief Getter of translation vector
 */
void Coordinate::getTranslation(float *newpos){
  memcpy(newpos,pos,sizeof(float)*3);
}
void Coordinate::getTranslation(vector <float> &newpos){
  newpos.resize(3);
  getTranslation(&newpos[0]);
}

/**
   @brief Setter of rotation matrix
 */
void Coordinate::setRotationMatrix(float *rmat){
  memcpy(rot,rmat,sizeof(float)*9);
}
void Coordinate::setRotationMatrix(vector <float> &rmat){
  setRotationMatrix(&rmat[0]);
}
/**
   @brief Getter of rotation matrix
 */
void Coordinate::getRotationMatrix(float *rmat){
  memcpy(rmat,rot,sizeof(float)*9);
}
void Coordinate::getRotationMatrix(vector <float> &rmat){
  rmat.resize(9);
  getRotationMatrix(&rmat[0]);
}

/**
   @brief Setting rotation matrix using quaternion
 */
void Coordinate::setRotationQuaternion(float *quat){
  float q00,q11,q22,q33;
  float q01,q02,q03,q12,q13,q23;

  q00=quat[0]*quat[0];
  q01=quat[0]*quat[1];
  q02=quat[0]*quat[2];
  q03=quat[0]*quat[3];

  q11=quat[1]*quat[1];
  q12=quat[1]*quat[2];
  q13=quat[1]*quat[3];

  q22=quat[2]*quat[2];
  q23=quat[2]*quat[3];

  q33=quat[3]*quat[3];


  rot[0]=q00+q11-q22-q33;
  rot[1]=2.0*(q12-q03);
  rot[2]=2.0*(q13+q02);

  rot[3]=2.0*(q12+q03);
  rot[4]=q00-q11+q22-q33;
  rot[5]=2.0*(q23-q01);

  rot[6]=2.0*(q13-q02);
  rot[7]=2.0*(q23+q01);
  rot[8]=q00-q11-q22+q33;
}
void Coordinate::setRotationQuaternion(vector<float> &quat){
  setRotationQuaternion(&quat[0]);
}
/**
   @brief Calcurating quaternion from rotation matrix
 */
void Coordinate::getRotationQuaternion(float *quat){
  float ww,xx,yy,zz;
  ww =   rot[0] + rot[4] + rot[8];
  xx =   rot[0] - rot[4] - rot[8];
  yy = - rot[0] + rot[4] - rot[8];
  zz = - rot[0] - rot[4] + rot[8];

  float maxv=ww;
  int idx=0;
  if(xx>maxv){
    maxv=xx;
    idx=1;
  }else if(yy>maxv){
    maxv=yy;
    idx=2;
  }else if(zz>maxv){
    maxv=zz;
    idx=3;
  }

  float val=0.5*(sqrt(maxv+1.0));
  float mult=0.25/val;

  switch(idx){
  case 0:
    quat[0]=val;
    quat[1]=(rot[5]-rot[7])*mult;
    quat[2]=(rot[6]-rot[2])*mult;
    quat[3]=(rot[1]-rot[3])*mult;
    break;
  case 1:
    quat[0]=(rot[5]-rot[7])*mult;
    quat[1]=val;
    quat[2]=(rot[1]+rot[3])*mult;
    quat[3]=(rot[6]+rot[2])*mult;
    break;
  case 2:
    quat[0]=(rot[6]-rot[2])*mult;
    quat[1]=(rot[1]+rot[3])*mult;
    quat[2]=val;
    quat[1]=(rot[5]+rot[7])*mult;
    break;
  case 3:
    quat[0]=(rot[1]-rot[3])*mult;
    quat[1]=(rot[6]+rot[2])*mult;
    quat[2]=(rot[5]+rot[7])*mult;
    quat[3]=val;
    break;
  }
}
void Coordinate::getRotationQuaternion(vector<float> &quat){
  quat.resize(4);
  getRotationQuaternion(&quat[0]);
}

/**
   @brief Translating RTC idl <-> Coordinate instance
*/
void Coordinate::fromTimedCoordinate(TimedCoordinate &rtc_co){
  if(rtc_co.transform.length()!=16){
    cerr << "[Coordinate::fromTimedFloatSeq] missmatch length." << endl;
    return;
  }
  float mat[16];
  for(int i=0;i<16;i++)
    mat[i]=rtc_co.transform[i];
  set4x4(mat);
  setTime(rtc_co.tm.sec,rtc_co.tm.nsec);
  rtc_co.parent_id=parent_id.c_str();
  rtc_co.frame_id=frame_id.c_str();
}
void Coordinate::toTimedCoordinate(TimedCoordinate &rtc_co){
  float mat[16];
  get4x4(mat);
  rtc_co.transform.length(16);
  for(int i=0;i<16;i++)
    rtc_co.transform[i]=mat[i];
  rtc_co.tm.sec=getSecond();
  rtc_co.tm.nsec=getNanoSecond();
  parent_id=string(rtc_co.parent_id);
  frame_id=string(rtc_co.frame_id);
}
