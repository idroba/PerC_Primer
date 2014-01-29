// 05_FingerDetection.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <Windows.h>
#include <iostream>

#include "util_pipeline.h"
#include <opencv2\opencv.hpp>

class Pipeline: public UtilPipeline {

public:

  Pipeline(void)
    : UtilPipeline()
  {
    // �K�v�ȃf�[�^��L���ɂ���
    EnableGesture();
    EnableImage(PXCImage::COLOR_FORMAT_RGB32, Width, Height);
  }

  // �V�����t���[��
  virtual bool OnNewFrame(void)
  {
    try {
      // �t���[�����擾����
      auto colorFrame = QueryImage( PXCImage::IMAGE_TYPE_COLOR );
      auto depthFrame = QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
      auto gestureFrame = QueryGesture();

      // �擾�����f�[�^��\������
      cv::Mat colorImage( Height, Width, CV_8UC4 );
      cv::Mat depthImage( DEPTH_HEIGHT, DEPTH_WIDTH, CV_8U );
      showColorFrame( colorImage, colorFrame );
      showDepthFrame( depthImage, depthFrame );
      showGestureFrame( colorImage, gestureFrame, colorFrame, depthFrame );

      // �\��
      cv::imshow( "Intel Perceptual Computing SDK", colorImage );
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
    auto sts = colorFrame->AcquireAccess( PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_RGB32, &data );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // RGB�f�[�^���擾����
    memcpy( colorImage.data, data.planes[0], data.pitches[0] * Height );

    // Color�f�[�^���������
    colorFrame->ReleaseAccess( &data );
  }

  // Depth�f�[�^��\������
  void showDepthFrame( cv::Mat& depthImage, PXCImage* depthFrame )
  {
    // Depth�f�[�^���擾����
    PXCImage::ImageData data = { 0 };
    auto sts = depthFrame->AcquireAccess( PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_DEPTH, &data );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // Depth�f�[�^����������
    ushort* srcDepth = (ushort*)data.planes[0];
    uchar* dstDepth = (uchar*)depthImage.data;
    for ( int i = 0; i < DEPTH_WIDTH * DEPTH_HEIGHT; ++i ) {
      if ( (150 <= srcDepth[i]) && (srcDepth[i] < 800) ) {
        dstDepth[i] = srcDepth[i] * 0xFF / 1000;
      }
      else {
        dstDepth[i] = 0xFF;
      }
    }

    // Depth�f�[�^���������
    depthFrame->ReleaseAccess( &data );
  }

  // �W�F�X�`���[�t���[����\������
  void showGestureFrame( cv::Mat& colorImage, PXCGesture* gestureFrame, PXCImage* colorFrame, PXCImage* depthFrame )
  {
    showHand( colorImage, gestureFrame, colorFrame, depthFrame, PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY );
    showHand( colorImage, gestureFrame, colorFrame, depthFrame, PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY );
  }

  // ��̈ʒu��\������
  void showHand( cv::Mat& colorImage, PXCGesture* gestureFrame, PXCImage* colorFrame, PXCImage* depthFrame, PXCGesture::GeoNode::Label hand )
  {
    // ��̈ʒu
    showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 255, 0, 0 ), hand );

    // ��̓I�Ȏw�̈ʒu
    showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 0, 255, 0 ),
      hand | PXCGesture::GeoNode::LABEL_FINGER_THUMB );
    showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 0, 0, 255 ),
      hand | PXCGesture::GeoNode::LABEL_FINGER_INDEX );
    showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 255, 255, 0 ),
      hand | PXCGesture::GeoNode::LABEL_FINGER_MIDDLE );
    showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 255, 0, 255 ),
      hand | PXCGesture::GeoNode::LABEL_FINGER_RING );
    showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 0, 255, 255 ),
      hand | PXCGesture::GeoNode::LABEL_FINGER_PINKY );

    // ���ۓI�Ȏw�̈ʒu
    //showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 0, 255, 0 ),
    //  hand | PXCGesture::GeoNode::LABEL_HAND_FINGERTIP );
    //showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 0, 0, 255 ),
    //  hand | PXCGesture::GeoNode::LABEL_HAND_UPPER );
    //showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 255, 255, 0 ),
    //  hand | PXCGesture::GeoNode::LABEL_HAND_MIDDLE );
    //showLabel( colorImage, gestureFrame, colorFrame, depthFrame, cv::Scalar( 255, 0, 255 ),
    //  hand | PXCGesture::GeoNode::LABEL_HAND_LOWER );
  }

  // �w�̈ʒu��\������
  void showLabel( cv::Mat& colorImage, PXCGesture* gestureFrame, PXCImage* colorFrame, PXCImage* depthFrame,
    cv::Scalar color, PXCGesture::GeoNode::Label label )
  {
    // �W�F�X�`���[�f�[�^���擾����
    PXCGesture::GeoNode nodeData = { 0 };
    auto sts = gestureFrame->QueryNodeData( 0 , label, &nodeData );
    if ( sts < PXC_STATUS_NO_ERROR) {
      return;
    }

    // Depth�f�[�^���擾����
    PXCImage::ImageData depthData = { 0 };
    sts = depthFrame->AcquireAccess( PXCImage::ACCESS_READ, &depthData );
    if ( sts < PXC_STATUS_NO_ERROR ) {
      return;
    }

    // X,Y���W����ʂ̍��W�ɕϊ�����
    PXCImage::ImageInfo rgbInfo = { 0, 0, 0 };
    PXCImage::ImageInfo depthInfo = { 0, 0, 0 };
    colorFrame->QueryInfo( &rgbInfo );
    depthFrame->QueryInfo( &depthInfo );
    auto x = nodeData.positionImage.x;
    auto y = nodeData.positionImage.y;
    MapXY( x, y, &depthData, &depthInfo, &rgbInfo );

    // ��ʏ�Ɉʒu�ɓ_��`�悷��
    cv::circle( colorImage, cv::Point( x, y ), 5, color, -1 );

    // Depth�f�[�^���������
    depthFrame->ReleaseAccess( &depthData );
  }

  // 3�������W��2�������W�ɕϊ�����
  static void MapXY(float &x, float &y, PXCImage::ImageData *data, PXCImage::ImageInfo *depthInfo, PXCImage::ImageInfo *rgbInfo) {
    if (data->planes[2]) {
      if (x>=0 && y>=0 &&  x<depthInfo->width && y<depthInfo->height) {
        int index=(int)(((int)y)*depthInfo->width+x);
        int index2=2*index;

        x=((float*)data->planes[2])[index2]*rgbInfo->width;
        y=((float*)data->planes[2])[index2+1]*rgbInfo->height;

        // X���W�𔽓]����
        //x = abs(x - rgbInfo->width);
      }
    } else {
      if (depthInfo->width>0 && depthInfo->height>0) {
        x=x/depthInfo->width*rgbInfo->width;
        y=y/depthInfo->height*rgbInfo->height;
      }
    }
  }

protected:

  //static const int Width = 640;
  //static const int Height = 480;
  static const int Width = 1280;
  static const int Height = 720;

  static const int DEPTH_WIDTH = 320;
  static const int DEPTH_HEIGHT = 240;

  PXCImage::ColorFormat colorFormat;
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

