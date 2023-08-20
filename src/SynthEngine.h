#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include "serial/serial.h"
#include <chrono>
#include <mutex>
#include <thread>
#include <portmidi.h>

#include "SynthParams.h"

#define SYNTH_NVOICES 6

#define SYNTH_SELECT_UPPER 0xF9
#define SYNTH_SELECT_LOWER 0xF1
#define SYNTH_SELECT_BOTH 0xF4

class SynthEngine {
public:
  SynthEngine();
  ~SynthEngine();
  bool init(const serial::PortInfo &port);

  void changeParam(bool upper, int param, int value);

  int toneU[256] = {0};
  int toneL[256] = {0};

  VoiceState voicesStateU[SYNTH_NVOICES];
  VoiceState voicesStateL[SYNTH_NVOICES];

  PolyMode polyMode;

private:
  int lastBoardSelected = 0x00;

  std::unique_ptr<serial::Serial> serialPort;
  std::mutex serialFdMutex;

  std::unique_ptr<std::thread> synthThreadPtr;
  bool running = false;

  PmStream *midiInStream;

  bool initSerial(const serial::PortInfo &port);
  bool initPortMidi();
  void threadStart();

  void noteOn(int note, int velocity);
  void noteOff(int note, int velocity);

  int getBestNewVoiceId(bool upper);
  void claimVoice(int voiceId, bool upper, int note, int velocity);
  void freeVoice(bool upper, int note, int velocity);
};

#endif
