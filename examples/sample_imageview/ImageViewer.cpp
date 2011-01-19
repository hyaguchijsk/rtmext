// -*- C++ -*-
/*!
 * @file  ImageViewer.cpp
 * @brief image viewer
 * @date $Date: 2010-02-02 19:39:01 +0900 (火, 02  2月 2010) $
 *
 * $Id: ImageViewer.cpp 318 2010-02-02 10:39:01Z eus $
 */

#include "ImageViewer.h"

// Module specification
// <rtc-template block="module_spec">
static const char* imageviewer_spec[] =
  {
    "implementation_id", "ImageViewer",
    "type_name",         "ImageViewer",
    "description",       "image viewer",
    "version",           "1.0.0",
    "vendor",            "Hiroaki Yaguchi",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    //"exec_cxt.periodic.rate", "1.0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageViewer::ImageViewer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ImageIn("Image", m_Image),

    // </rtc-template>
	dummy(0)
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers


  namedWindow("Image",CV_WINDOW_AUTOSIZE);

  waitKey(10);
}

/*!
 * @brief destructor
 */
ImageViewer::~ImageViewer()
{
}


RTC::ReturnCode_t ImageViewer::onInitialize()
{
  registerInPort("Image", m_ImageIn);
  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageViewer::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ImageViewer::onExecute(RTC::UniqueId ec_id)
{
  int width,height,depth;
  int idx;

  if(m_ImageIn.isNew()){
    m_ImageIn.read();

    width=m_Image.width;
    height=m_Image.height;
    depth=m_Image.bpp/8;

    cerr << "receieved image: " << width << " x " << height << " x " << depth  << endl;

    Mat img;

    if(convertToCV(m_Image,img))
      imshow("Image",img);
    else
      cerr << "convert failed." <<endl;
  }

  waitKey(10);
  return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t ImageViewer::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


extern "C"
{
 
  void ImageViewerInit(RTC::Manager* manager)
  {
    //RTC::Properties profile(imageviewer_spec);
    coil::Properties profile(imageviewer_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageViewer>,
                             RTC::Delete<ImageViewer>);
  }
  
};


