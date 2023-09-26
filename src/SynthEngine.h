#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include "serial/serial.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <portmidi.h>
#include <thread>

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
  void resendAll();

  int toneU[256] = {0};
  int toneL[256] = {0};

  VoiceState voicesStateU[SYNTH_NVOICES];
  VoiceState voicesStateL[SYNTH_NVOICES];

  PolyMode polyMode;
  PatchMode patchMode = Dual;

private:
  int lastBoardSelected = 0x00;
  bool lastVoiceUpper = false;

  int programmerLast[256] = {0};

  std::unique_ptr<serial::Serial> serialPort;
  std::unique_ptr<serial::Serial> serialPortProgrammer;
  std::mutex serialFdMutex;

  std::unique_ptr<std::thread> synthThreadPtr;
  std::unique_ptr<std::thread> programmerThreadPtr;
  bool running = false;

  PmStream *midiInStream;

  bool initSerial(const serial::PortInfo &port);
  bool initPortMidi();
  void threadStart();
  void programmerThreadStart();

  void noteOn(int note, int velocity);
  void noteOff(int note, int velocity);

  int getBestNewVoiceId(bool upper, int note);
  void claimVoice(int voiceId, bool upper, int note, int velocity);
};

#endif
