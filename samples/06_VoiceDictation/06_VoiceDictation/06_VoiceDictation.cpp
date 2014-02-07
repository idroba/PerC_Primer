// 06_VoiceDictation.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <iostream>
#include <Windows.h>

#include "util_pipeline.h"

class Pipeline: public UtilPipeline {
public:

  // 1.�R���X�g���N�^
  Pipeline(void)
    : UtilPipeline()
  {
    // �K�v�ȃf�[�^��L���ɂ���
    EnableVoiceRecognition();
  }

  // 2.�����F���̃Z�b�g�A�b�v���s��
  virtual void  OnVoiceRecognitionSetup(PXCVoiceRecognition::ProfileInfo * finfo)
  {
    // ���{��̉����G���W����T��
    auto voiceRecognition = QueryVoiceRecognition();
    for ( int i = 0; ; ++i ) {
      PXCVoiceRecognition::ProfileInfo pinfo = { 0 };
      auto ret = voiceRecognition->QueryProfile( i, &pinfo );
      if ( ret != PXC_STATUS_NO_ERROR ) {
        break;
      }

      if ( pinfo.language == PXCVoiceRecognition::ProfileInfo::LANGUAGE_JP_JAPANESE ) {
        *finfo = pinfo;
        std::cout << "���{��̉����G���W����ݒ肵�܂���" << std::endl;
      }
    }

    std::cout << "�����F�����J�n���܂�" << std::endl;
  }

  // 3.����F������ƌĂяo�����
  virtual void PXCAPI OnRecognized( PXCVoiceRecognition::Recognition *cmd ) {
    std::wcout << L"�F��������: " << cmd->dictation << std::endl;
  }
};

int _tmain(int argc, _TCHAR* argv[])
{
  try {
    // 4.wide-charactor��\���ł���悤�ɂ���
    std::locale::global(std::locale("japanese"));

    Pipeline pipeline;
    pipeline.LoopFrames();
  }
  catch ( std::exception& ex ) {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}

