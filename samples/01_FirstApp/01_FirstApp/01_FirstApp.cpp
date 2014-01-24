// 01_FirstApp.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <iostream>

#include "util_pipeline.h"
#include <opencv2\opencv.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
  PXCSession *session = 0;
  PXCSession_Create( &session );

  PXCSession::ImplVersion version = { 0 };
  session->QueryVersion( &version );

  std::cout << version.major << "." << version.minor << std::endl;

  return 0;
}
