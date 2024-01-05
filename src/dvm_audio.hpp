#include <portaudio.h>
#include <iostream>

#define SAMPLE_RATE 44100

#define PA_CHECK(err) \
  if (err != paNoError) { \
    printf("PortAudio error: %s\n", Pa_GetErrorText(err)); \
  }

typedef struct
{
  float left_phase;
  float right_phase;
} paTestData;

static paTestData data;

namespace dvm
{
static int patestCallback(const void* inputBuffer,
                          void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData)
{
  /* Cast data passed through stream to our structure. */
  paTestData* data = (paTestData*)userData;
  float* out = (float*)outputBuffer;
  unsigned int i;
  (void)inputBuffer; /* Prevent unused variable warning. */

  for (i = 0; i < framesPerBuffer; i++) {
    *out++ = data->left_phase * 0.05f; /* left */
    *out++ = data->right_phase * 0.05f; /* right */
    /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
    data->left_phase += 0.006f;
    /* When signal reaches top, drop back down. */
    if (data->left_phase >= 1.0f)
      data->left_phase -= 2.0f;
    /* higher pitch so we can distinguish left and right. */
    data->right_phase += 0.01f;
    if (data->right_phase >= 1.0f)
      data->right_phase -= 2.0f;
  }
  return 0;
}
class DvmAudio
{
public:
  static DvmAudio* getInstance()
  {
    if (s_instance == nullptr) {
      s_instance = new DvmAudio();
    }
    return s_instance;
  }

  void operator()()
  {
    printf("Initializing audio..");
    PaError err;
    err = Pa_Initialize();
    PA_CHECK(err);

    PaStream* stream;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(
        &stream,
        0, /* no input channels */
        2, /* stereo output */
        paFloat32, /* 32 bit floating point output */
        SAMPLE_RATE,
        paFramesPerBufferUnspecified, /* frames per buffer, i.e. the number
                    of sample frames that PortAudio will
                    request from the callback. Many apps
                    may want to use
                    paFramesPerBufferUnspecified, which
                    tells PortAudio to pick the best,
                    possibly changing, buffer size.*/
        patestCallback, /* this is your callback function */
        &data); /*This is a pointer that will be passed to
                          your callback*/
    PA_CHECK(err);

    err = Pa_StartStream(stream);
    PA_CHECK(err);
    Pa_Sleep(6000);
    Pa_StopStream(stream);
  }
  ~DvmAudio()
  {
    PaError err = Pa_Terminate();
    PA_CHECK(err);
  }

private:
  static DvmAudio* s_instance;
};
}  // namespace dvm