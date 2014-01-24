// 03_FaceDetection.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <iostream>

#include "util_pipeline.h"
#include <opencv2\opencv.hpp>

class Pipeline: public UtilPipeline {
protected:

  //static const int Width = 640;
  //static const int Height = 480;
  static const int Width = 1280;
  static const int Height = 720;
  //static const int Width = 1920;
  //static const int Height = 1080;

  PXCImage::ColorFormat colorFormat;

public:

  Pipeline(void)
    : UtilPipeline()
    , colorFormat( PXCImage::COLOR_FORMAT_RGB32 )
  {
    // �K�v�ȃf�[�^��L���ɂ���
    EnableImage( colorFormat, Width, Height );

    // ��̌��o��L���ɂ���
    EnableFaceLocation();
  }

  // �V�����t���[��
  virtual bool OnNewFrame(void)
  {
    try {
      // �t���[�����擾����
      auto colorFrame = QueryImage( PXCImage::IMAGE_TYPE_COLOR );
      auto faceFrame = QueryFace();

      // �擾�����f�[�^��\������
      cv::Mat colorImage( Height, Width, CV_8UC4 );
      showColorFrame( colorImage, colorFrame );
      showFaceDetection( colorImage, faceFrame );
      
      // �摜�����E���]����
      //cv::flip( colorImage, colorImage, 1 );

      // �\��
      cv::imshow( "Color Camera", colorImage );
    }
    catch ( std::exception& ex ) {
      std::cout << ex.what() << std::endl;
    }

    auto key = cv::waitKey( 10 );
    return key != 'q';
  }

  // Color�f�[�^��\������
  void showColorFrame( cv::Mat& colorImage, PXCImage* colorFrame )
  {
    // Color�f�[�^���擾����
    PXCImage::ImageData data = { 0 };
    auto sts = colorFrame->AcquireAccess( PXCImage::ACCESS_READ, colorFormat, &data );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // Color�f�[�^����������
    memcpy( colorImage.data, data.planes[0], data.pitches[0] * Height );

    // Color�f�[�^���������
    colorFrame->ReleaseAccess( &data );
  }

  // ���o������̈ʒu��\������
  void showFaceDetection( cv::Mat& colorImage, PXCFaceAnalysis* faceFrame )
  {
    auto detector = faceFrame->DynamicCast<PXCFaceAnalysis::Detection>();
    for ( int i = 0 ;; ++i ) {
      // ���[�U�[�̊�ID���擾����
      pxcUID fid = 0; 
      pxcU64 timeStampe = 0;
      auto sts = faceFrame->QueryFace( i, &fid, &timeStampe );
      if ( sts < PXC_STATUS_NO_ERROR ) {
        break;
      }

      // ��̃f�[�^���擾����
      PXCFaceAnalysis::Detection::Data data;
      detector->QueryData(fid,&data);

      // ��̈ʒu��`�悷��
      cv::rectangle( colorImage,
        cv::Rect( data.rectangle.x, data.rectangle.y, data.rectangle.w, data.rectangle.h ),
        cv::Scalar( 255, 0, 0 ) );
    }
  }
};

int _tmain(int argc, _TCHAR* argv[])
{
  try {
    Pipeline pipeline;
    pipeline.LoopFrames();
  }
  catch ( std::exception& ex ) {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}

