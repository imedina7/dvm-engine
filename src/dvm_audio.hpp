#pragma once
#include <portaudio.h>
#include <sndfile.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>

#define SAMPLE_RATE 44100
#define AUDIO_FILE_PATH "../audio/test.wav"
#define MASTER_VOLUME 0.1f

#define BUFFER_SIZE 1024

#define PA_CHECK(err) \
  if (err != paNoError) { \
    printf("PortAudio error: %s\n", Pa_GetErrorText(err)); \
  }

typedef struct
{
  float volume;
  uint32_t maxFrames;
  uint32_t frameIndex;
  SNDFILE* sfHandle;
  SF_INFO* sfInfo;
} StreamData;

enum SoundFX
{
  SWOOSH = 0,
  NUM_SOUND_FX
};

namespace dvm
{
class DvmAudio
{
public:
  DvmAudio(const DvmAudio&) = delete;

  static DvmAudio& Get() { return s_instance; }

  // void playSoundFX(SoundFX fx);
  void playFromFile(const std::string filename);
  ~DvmAudio()
  {
    PaError err = Pa_Terminate();
    PA_CHECK(err);
  }

private:
  DvmAudio()
  {
    PaError err;
    err = Pa_Initialize();
    PA_CHECK(err);
  }
  static DvmAudio s_instance;
  std::thread audioThread;
  StreamData streamData;
};
}  // namespace dvm