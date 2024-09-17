#include "dvm_audio.hpp"

namespace dvm
{
DvmAudio DvmAudio::s_instance;

static int paCallback(const void* inputBuffer,
                      void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData)
{
  /* Cast data passed through stream to our structure. */
  StreamData* data = (StreamData*)userData;
  float* out = (float*)outputBuffer;
  unsigned int i;
  (void)inputBuffer;

  uint32_t remainingFrames = data->maxFrames - data->frameIndex;
  if (remainingFrames == 0)
    return 1;

  uint32_t frameBatch =
      (remainingFrames < framesPerBuffer) ? remainingFrames : framesPerBuffer;

  sf_seek(data->sfHandle, data->frameIndex, SEEK_SET);

  data->frameIndex += sf_readf_float(data->sfHandle, out, frameBatch);

  *out = *out * data->volume;

  return 0;
}

void DvmAudio::playFromFile(const std::string filename)
{
  SF_INFO sfInfo;
  sfInfo.format = 0;

  SNDFILE* sf = sf_open(filename.data(), SFM_READ, &sfInfo);

  uint64_t audioLengthSeconds = sfInfo.frames / sfInfo.samplerate;
  StreamData streamData;
  streamData.volume = .0f;
  streamData.maxFrames = sfInfo.frames;
  streamData.frameIndex = 0;
  streamData.sfHandle = sf;
  streamData.sfInfo = &sfInfo;

  PaError err;

  std::cout << "Audio file name: \"" << filename << "\"\n";
  std::cout << "Audio file length: " << audioLengthSeconds << " seconds\n";
  std::cout << "Audio sample rate: " << sfInfo.samplerate << "hz\n";
  std::cout << "Audio channels: " << sfInfo.channels << "\n";

  PaStream* defaultStream;
  /* Open an audio I/O stream. */
  std::cout << "Opening Stream...\n";
  err =
      Pa_OpenDefaultStream(&defaultStream,
                           0, /* no input channels */
                           sfInfo.channels, /* stereo output */
                           paFloat32, /* 32 bit floating point output */
                           sfInfo.samplerate,
                           paFramesPerBufferUnspecified, /* frames per buffer,
                           i.e. the number of sample frames that PortAudio will
                                       request from the callback. Many apps
                                       may want to use
                                       paFramesPerBufferUnspecified, which
                                       tells PortAudio to pick the best,
                                       possibly changing, buffer size.*/
                           paCallback, /* this is your callback function */
                           &streamData); /*This is a pointer that will be
                                            passed to your callback*/
  PA_CHECK(err);
  std::cout << "Starting Stream...\n";
  err = Pa_StartStream(defaultStream);
  PA_CHECK(err);

  Pa_Sleep(audioLengthSeconds * 1000);
  sf_close(sf);

  Pa_StopStream(defaultStream);
  Pa_CloseStream(defaultStream);
}

// void DvmAudio::playSoundFX(SoundFX fx)
// {
//   switch (fx) {
//     case SWOOSH:
//     default:
//       audioThread = std::thread(;
//       break;
//   }
// }
}  // namespace dvm