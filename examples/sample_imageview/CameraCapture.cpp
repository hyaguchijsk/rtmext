// -*- C++ -*-
/*!
 * @file  CameraCapture.cpp
 * @brief capture image from camera
 * @date $Date: 2010-02-02 19:38:42 +0900 (火, 02  2月 2010) $
 *
 * $Id: CameraCapture.cpp 317 2010-02-02 10:38:42Z eus $
 */

#include "CameraCapture.h"

// Module specification
// <rtc-template block="module_spec">
static const char* cameracapture_spec[] =
  {
    "implementation_id", "CameraCapture",
    "type_name",         "CameraCapture",
    "description",       "capture image from camera",
    "version",           "1.0.0",
    "vendor",            "Hiroaki Yaguchi JSK",
    "category",          "Image Processing",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    //"exec_cxt.periodic.rate", "1.0",
    // Configuration variables
    "conf.default.DeviceID", "0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CameraCapture::CameraCapture(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ImageOut("Image", m_Image),

    // </rtc-template>
	dummy(0)
{
}

/*!
 * @brief destructor
 */
CameraCapture::~CameraCapture()
{
}



RTC::ReturnCode_t CameraCapture::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  registerOutPort("Image", m_ImageOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("DeviceID", m_DeviceID, "0");
  // </rtc-template>

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CameraCapture::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CameraCapture::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CameraCapture::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t CameraCapture::onActivated(RTC::UniqueId ec_id)
{
  cap=new VideoCapture(m_DeviceID);
  if(!cap->isOpened()){
    cerr << "DeviceID " << m_DeviceID << " : cannot open." << endl;
    return RTC::RTC_ERROR;
  }
  cap->set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap->set(CV_CAP_PROP_FRAME_HEIGHT,480);
  return RTC::RTC_OK;
}



RTC::ReturnCode_t CameraCapture::onDeactivated(RTC::UniqueId ec_id)
{
  delete cap;
  return RTC::RTC_OK;
}



RTC::ReturnCode_t CameraCapture::onExecute(RTC::UniqueId ec_id)
{
  *cap >> img;
  //convertToRTM(img,m_Image);
  encodeImageToRTM(".jpg",img,m_Image);
  m_ImageOut.write();
  //waitKey(33);

  return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t CameraCapture::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CameraCapture::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CameraCapture::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CameraCapture::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CameraCapture::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


extern "C"
{
 
  void CameraCaptureInit(RTC::Manager* manager)
  {
    //RTC::Properties profile(cameracapture_spec);
    coil::Properties profile(cameracapture_spec);
    manager->registerFactory(profile,
                             RTC::Create<CameraCapture>,
                             RTC::Delete<CameraCapture>);
  }
  
};


