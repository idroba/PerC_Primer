// 08_VoiceSynthesis.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <iostream>

#include "util_pipeline.h"
#include "voice_out.h"
#pragma comment( lib, "winmm.lib" )

int _tmain(int argc, _TCHAR* argv[])
{
  // wide-charactor��������悤�ɂ���
  std::locale::global(std::locale("japanese"));

  UtilPipeline pipeline;
  pipeline.Init();

  PXCVoiceSynthesis* synthesis = nullptr;
  pipeline.QuerySession()->CreateImpl<PXCVoiceSynthesis>( &synthesis );

  PXCVoiceSynthesis::ProfileInfo pinfo = { 0 };
  for ( int i = 0; ; ++i ) {
    auto ret = synthesis->QueryProfile( i, &pinfo );
    if ( ret != PXC_STATUS_NO_ERROR ) {
      break;
    }

    if ( pinfo.language == PXCVoiceRecognition::ProfileInfo::LANGUAGE_JP_JAPANESE ) {
      synthesis->SetProfile( &pinfo );
      std::cout << "���{��̉����G���W����ݒ肵�܂���" << std::endl;
      break;
    }
  }

  VoiceOut voice( &pinfo );

  while ( 1 ) {
    std::cout << "���͂��Ă��������F";
    std::wstring message;
    std::wcin >> message;

    pxcUID id=0;
    synthesis->QueueSentence( (wchar_t*)message.c_str(), message.size(), &id );

    for (;;) {
      PXCSmartSP sp;
      PXCAudio *sample;

      // Request audio data from TTS
      auto ret = synthesis->ProcessAudioAsync(id, &sample, &sp);
      if ( ret<PXC_STATUS_NO_ERROR ) {
        break;
      }

      // Make sure there is data that is valid
      ret = sp->Synchronize();
      if ( ret<PXC_STATUS_NO_ERROR ) {
        break;
      }

      voice.RenderAudio(sample);
    }
  }


  return 0;
}

