// -*- C++ -*-
/*!
 * @file  ImageFileCapture.cpp
 * @brief read image from file
 * @date $Date: 2010-02-02 19:38:42 +0900 (火, 02  2月 2010) $
 *
 * $Id: ImageFileCapture.cpp 317 2010-02-02 10:38:42Z eus $
 */

#include "ImageFileCapture.h"

// Module specification
// <rtc-template block="module_spec">
static const char* imagefilecapture_spec[] =
  {
    "implementation_id", "ImageFileCapture",
    "type_name",         "ImageFileCapture",
    "description",       "read image from file",
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
    "conf.default.ImageDir", "image",
    "conf.default.ImagePrefix", "image-cap",
    "conf.default.ImageSuffix", "jpg",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageFileCapture::ImageFileCapture(RTC::Manager* manager)
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
ImageFileCapture::~ImageFileCapture()
{
}



RTC::ReturnCode_t ImageFileCapture::onInitialize()
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
  bindParameter("ImageDir", m_ImageDir, "image");
  bindParameter("ImagePrefix", m_ImagePrefix, "image-cap");
  bindParameter("ImageSuffix", m_ImageSuffix, "jpg");
  
  // </rtc-template>

  imageindex=0;

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageFileCapture::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ImageFileCapture::onExecute(RTC::UniqueId ec_id)
{
  bool ret=loadImage();
  if(!ret){
    cerr << string(imagefilename) << " not found. return image index to 0." << endl;
    imageindex=0;
    ret=loadImage();
  }

  if(ret){
    if(m_ImageSuffix=="jpg"){
      m_Image.format=".jpg";
      m_Image.bpp=0;
      m_Image.pixels.length(jpegbuf.size());
      for(int i=0;i<jpegbuf.size();i++)
	m_Image.pixels[i]=jpegbuf[i];
    }else{
      convertToRTM(img,m_Image);
    }
    m_ImageOut.write();
    cerr << "send: " << string(imagefilename) << endl;
  }else{
    cerr << string(imagefilename) << " not found." << endl;
  }

  imageindex++;

  return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t ImageFileCapture::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageFileCapture::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

bool ImageFileCapture::loadImage(){
  sprintf(imagefilename,
          "%s/%s-%07d.%s",
          m_ImageDir.c_str(),
          m_ImagePrefix.c_str(),
          imageindex,
          m_ImageSuffix.c_str());

  if(m_ImageSuffix=="jpg"){
    ifstream fin(imagefilename,ios::in | ios::binary);
    if(!fin) return false;
    size_t filesize = (size_t)fin.seekg(0, ios::end).tellg();
    fin.seekg(0, ios::beg);
    jpegbuf.resize(filesize);
    fin.read((char *)&jpegbuf[0],filesize);
    return true;
  }else{
    img=imread(imagefilename);
    if(img.data==NULL) return false;
    return true;
  }
}

extern "C"
{
 
  void ImageFileCaptureInit(RTC::Manager* manager)
  {
    //RTC::Properties profile(imagefilecapture_spec);
    coil::Properties profile(imagefilecapture_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageFileCapture>,
                             RTC::Delete<ImageFileCapture>);
  }
  
};


