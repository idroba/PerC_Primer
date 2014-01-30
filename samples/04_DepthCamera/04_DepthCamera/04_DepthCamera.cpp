// 04_DepthCamera.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <iostream>

// �w�b�_�[�t�@�C��
#include "util_pipeline.h"
#include <opencv2\opencv.hpp>

// UtilPipeline����p�������N���X
class Pipeline: public UtilPipeline {
protected:

  // 1. �萔�A�ϐ��̐錾 
  //static const int Width = 640;
  //static const int Height = 480;
  static const int Width = 1280;
  static const int Height = 720;

  // Depth�J�����̉𑜓x
  static const int DEPTH_WIDTH = 320;   // �ǉ�
  static const int DEPTH_HEIGHT = 240;  // �ǉ�

  PXCImage::ColorFormat colorFormat;

public:

  // 2. �R���X�g���N�^�[
  Pipeline(void)
    : UtilPipeline()
    , colorFormat( PXCImage::COLOR_FORMAT_RGB32 )
  {
    // �K�v�ȃf�[�^��L���ɂ���
    EnableImage( colorFormat, Width, Height);
    EnableImage( PXCImage::COLOR_FORMAT_DEPTH, DEPTH_WIDTH, DEPTH_HEIGHT);
  }

  // 3. �V�����t���[���̍X�V�C�x���g
  virtual bool OnNewFrame(void)
  {
    try {
      // �t���[�����擾����
      auto colorFrame = QueryImage( PXCImage::IMAGE_TYPE_COLOR );
      auto depthFrame = QueryImage( PXCImage::IMAGE_TYPE_DEPTH );

      // �t���[���f�[�^���擾����
      cv::Mat colorImage( Height, Width, CV_8UC4 );
      cv::Mat depthImage;
      getColorData( colorImage, colorFrame );
      getDepthData( depthImage, depthFrame );
      
      // �\��
      cv::imshow( "Color Camera", colorImage );
      cv::imshow( "Depth Camera", depthImage );
    }
    catch ( std::exception& ex ) {
      std::cout << ex.what() << std::endl;
    }

    auto key = cv::waitKey( 10 );
    return key != 'q';
  }

  // Color�f�[�^���擾����
  void getColorData( cv::Mat& colorImage, PXCImage* colorFrame )
  {
    // Color�f�[�^���擾����
    PXCImage::ImageData data = { 0 };
    auto sts = colorFrame->AcquireAccess( PXCImage::ACCESS_READ,
                                          colorFormat, &data );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // Color�f�[�^����������
    memcpy( colorImage.data, data.planes[0], data.pitches[0] * Height );

    // Color�f�[�^���������
    colorFrame->ReleaseAccess( &data );
  }

  // 4. Depth�J�����̃f�[�^���擾����
  void getDepthData( cv::Mat& depthImage, PXCImage* depthFrame )
  {
#if 0
    // Depth�f�[�^���擾����
    PXCImage::ImageData data = { 0 };
    auto sts = depthFrame->AcquireAccess( PXCImage::ACCESS_READ,
                                          PXCImage::COLOR_FORMAT_DEPTH, &data );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // 8bit��Gray�摜���쐬����
    depthImage = cv::Mat( DEPTH_HEIGHT, DEPTH_WIDTH, CV_8U );

    // Depth�f�[�^����������
    ushort* srcDepth = (ushort*)data.planes[0];
    uchar* dstDepth = (uchar*)depthImage.data;
    for ( int i = 0; i < (DEPTH_WIDTH * DEPTH_HEIGHT); ++i ) {
      // ���̋����̂ݗL���ɂ���
      if ( (150 <= srcDepth[i]) && (srcDepth[i] < 800) ) {
        dstDepth[i] = srcDepth[i] * 0xFF / 1000;
      }
      else {
        dstDepth[i] = 0xFF;
      }
    }

    // Depth�f�[�^���������
    depthFrame->ReleaseAccess( &data );
#else
    // Depth�f�[�^���擾����
    PXCImage::ImageData data = { 0 };
    auto sts = depthFrame->AcquireAccess( PXCImage::ACCESS_READ,
                                          PXCImage::COLOR_FORMAT_RGB32, &data );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // 32bit��RGB�摜���쐬����
    depthImage = cv::Mat( DEPTH_HEIGHT, DEPTH_WIDTH, CV_8UC4 );

    // Depth�f�[�^����������
    memcpy( depthImage.data, data.planes[0], data.pitches[0] * DEPTH_HEIGHT );

    // Depth�f�[�^���������
    depthFrame->ReleaseAccess( &data );
#endif
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

