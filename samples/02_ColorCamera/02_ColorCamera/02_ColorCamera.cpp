// 02_ColorCamera.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <iostream>

// 1. �w�b�_�[�t�@�C��
#include "util_pipeline.h"
#include <opencv2\opencv.hpp>

// 2. UtilPipeline����p�������N���X
class Pipeline: public UtilPipeline {
protected:

  // 3. �萔�A�ϐ��̐錾 
  //static const int Width = 640;
  //static const int Height = 480;
  static const int Width = 1280;
  static const int Height = 720;

  PXCImage::ColorFormat colorFormat;

public:

  // 4. �R���X�g���N�^
  Pipeline()
    : UtilPipeline()
    , colorFormat( PXCImage::COLOR_FORMAT_RGB32 )
  {
    // �K�v�ȃf�[�^��L���ɂ���
    EnableImage( colorFormat, Width, Height );
  }

  // 5. �V�����t���[���̍X�V�C�x���g
  virtual bool OnNewFrame()
  {
    try {
      // �t���[�����擾����
      auto colorFrame = QueryImage( PXCImage::IMAGE_TYPE_COLOR );

      // �f�[�^���擾����
      cv::Mat colorImage( Height, Width, CV_8UC4 );
      getColorData( colorImage, colorFrame );

      // �摜�����E���]����
      //cv::flip( colorImage, colorImage, 1 );

      // �\������
      cv::imshow( "Color Camera", colorImage );
    }
    catch ( std::exception& ex ) {
      std::cout << ex.what() << std::endl;
    }

    auto key = cv::waitKey( 10 );
    return key != 'q';
  }

  // 6. Color�f�[�^���擾����
  void getColorData( cv::Mat& colorImage, PXCImage* colorFrame )
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
};

// 7. main�֐�
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

