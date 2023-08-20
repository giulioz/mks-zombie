#include "SynthEngine.h"

static unsigned char pianoPatchData[] = {
    0xF4, 0x9E, 0x00, 0xA6, 0x00, 0xA7, 0x00, 0xAB, 0x00, 0xAC, 0x00, 0xF1,
    0x80, 0x60, 0x81, 0x20, 0x82, 0x7F, 0x83, 0x00, 0x84, 0x7F, 0x85, 0x60,
    0x86, 0x20, 0x87, 0x60, 0x88, 0x5C, 0x89, 0x4D, 0x8A, 0x00, 0x8B, 0x00,
    0x8F, 0x00, 0x90, 0x60, 0x91, 0x00, 0x92, 0x7F, 0x93, 0x00, 0x94, 0x00,
    0x95, 0x60, 0x96, 0x00, 0x97, 0x44, 0x98, 0x6C, 0x99, 0x00, 0x9A, 0x00,
    0x9B, 0x5D, 0x9C, 0x20, 0x9D, 0x60, 0x9F, 0x60, 0xA0, 0x00, 0xA1, 0x00,
    0xA2, 0x00, 0xA3, 0x50, 0xA4, 0x00, 0xA5, 0x00, 0xA6, 0x00, 0xA7, 0x00,
    0xA8, 0x00, 0xA9, 0x00, 0xAA, 0x29, 0xAB, 0x00, 0xAC, 0x20, 0xAD, 0x20,
    0xAE, 0x00, 0xAF, 0x40, 0xF1, 0xB5, 0x00, 0xF9, 0x80, 0x20, 0x81, 0x60,
    0x82, 0x00, 0x83, 0x00, 0x84, 0x00, 0x85, 0x60, 0x86, 0x20, 0x87, 0x00,
    0x88, 0x76, 0x89, 0x00, 0x8A, 0x00, 0x8B, 0x00, 0x8F, 0x40, 0x90, 0x60,
    0x91, 0x52, 0x92, 0x42, 0x93, 0x7F, 0x94, 0x60, 0x95, 0x60, 0x96, 0x00,
    0x97, 0x2C, 0x98, 0x1B, 0x99, 0x00, 0x9A, 0x14, 0x9B, 0x62, 0x9C, 0x40,
    0x9D, 0x60, 0x9F, 0x20, 0xA0, 0x20, 0xA1, 0x20, 0xA2, 0x00, 0xA3, 0x67,
    0xA4, 0x00, 0xA5, 0x40, 0xA6, 0x30, 0xA7, 0x50, 0xA8, 0x20, 0xA9, 0x00,
    0xAA, 0x4F, 0xAB, 0x00, 0xAC, 0x29, 0xAD, 0x20, 0xAE, 0x7F, 0xAF, 0x40,
    0xF9, 0xB5, 0x00, 0xF1, 0x9E, 0x5C, 0xF9, 0x9E, 0x74, 0xF4, 0xB0, 0x01,
    0xF4, 0xB7, 0x00, 0xF4, 0xB8, 0x00, 0xB9, 0x00, 0xBA, 0x00, 0x8C, 0x00,
    0x8D, 0x00, 0x8E, 0x00, 0xB0, 0x01, 0xF1, 0xB6, 0x7F, 0xF9, 0xB6, 0x7F,
    0xF1, 0xBB, 0x00, 0xF9, 0xBB, 0x00, 0xF1, 0xBD, 0x00, 0xB4, 0x51, 0xBE,
    0x51, 0xF9, 0xBD, 0x00, 0xB4, 0x32, 0xBE, 0x32, 0xF9, 0xB4, 0x32, 0xBE,
    0x32, 0xF1, 0xB4, 0x51, 0xBE, 0x51, 0xF9, 0xB5, 0x00, 0xF1, 0xB5, 0x00,
    0xF5, 0xC0, 0x00, 0xF4, 0x9E, 0x00, 0xA6, 0x00, 0xA7, 0x00, 0xAB, 0x00,
    0xAC, 0x00, 0xF1, 0x80, 0x60, 0x81, 0x20, 0x82, 0x7F, 0x83, 0x00, 0x84,
    0x7F, 0x85, 0x60, 0x86, 0x20, 0x87, 0x60, 0x88, 0x5C, 0x89, 0x4D, 0x8A,
    0x00, 0x8B, 0x00, 0x8F, 0x00, 0x90, 0x60, 0x91, 0x00, 0x92, 0x7F, 0x93,
    0x00, 0x94, 0x00, 0x95, 0x60, 0x96, 0x00, 0x97, 0x44, 0x98, 0x6C, 0x99,
    0x00, 0x9A, 0x00, 0x9B, 0x5D, 0x9C, 0x20, 0x9D, 0x60, 0x9F, 0x60, 0xA0,
    0x00, 0xA1, 0x00, 0xA2, 0x00, 0xA3, 0x50, 0xA4, 0x00, 0xA5, 0x00, 0xA6,
    0x00, 0xA7, 0x00, 0xA8, 0x00, 0xA9, 0x00, 0xAA, 0x29, 0xAB, 0x00, 0xAC,
    0x20, 0xAD, 0x20, 0xAE, 0x00, 0xAF, 0x40, 0xF1, 0xB5, 0x00, 0xF9, 0x80,
    0x20, 0x81, 0x60, 0x82, 0x00, 0x83, 0x00, 0x84, 0x00, 0x85, 0x60, 0x86,
    0x20, 0x87, 0x00, 0x88, 0x76, 0x89, 0x00, 0x8A, 0x00, 0x8B, 0x00, 0x8F,
    0x40, 0x90, 0x60, 0x91, 0x52, 0x92, 0x42, 0x93, 0x7F, 0x94, 0x60, 0x95,
    0x60, 0x96, 0x00, 0x97, 0x2C, 0x98, 0x1B, 0x99, 0x00, 0x9A, 0x14, 0x9B,
    0x62, 0x9C, 0x40, 0x9D, 0x60, 0x9F, 0x20, 0xA0, 0x20, 0xA1, 0x20, 0xA2,
    0x00, 0xA3, 0x67, 0xA4, 0x00, 0xA5, 0x40, 0xA6, 0x30, 0xA7, 0x50, 0xA8,
    0x20, 0xA9, 0x00, 0xAA, 0x4F, 0xAB, 0x00, 0xAC, 0x29, 0xAD, 0x20, 0xAE,
    0x7F, 0xAF, 0x40, 0xF9, 0xB5, 0x00, 0xF1, 0x9E, 0x5C, 0xF9, 0x9E, 0x74,
    0xF4, 0xB0, 0x01, 0xF4, 0xB7, 0x00, 0xF4, 0xB8, 0x00, 0xB9, 0x00, 0xBA,
    0x00, 0x8C, 0x00, 0x8D, 0x00, 0x8E, 0x00, 0xB0, 0x01, 0xF1, 0xB6, 0x7F,
    0xF9, 0xB6, 0x7F, 0xF1, 0xBB, 0x00, 0xF9, 0xBB, 0x00, 0xF1, 0xBD, 0x00,
    0xB4, 0x51, 0xBE, 0x51, 0xF9, 0xBD, 0x00, 0xB4, 0x32, 0xBE, 0x32, 0xF9,
    0xB4, 0x32, 0xBE, 0x32, 0xF1, 0xB4, 0x51, 0xBE, 0x51, 0xF9, 0xB5, 0x00,
    0xF1, 0xB5, 0x00, 0xF5, 0xC0, 0x00, 0xF4, 0x9E, 0x00, 0xA6, 0x00, 0xA7,
    0x00, 0xAB, 0x00, 0xAC, 0x00, 0xF1, 0x80, 0x60, 0x81, 0x20, 0x82, 0x7F,
    0x83, 0x00, 0x84, 0x7F, 0x85, 0x60, 0x86, 0x20, 0x87, 0x60, 0x88, 0x5C,
    0x89, 0x4D, 0x8A, 0x00, 0x8B, 0x00, 0x8F, 0x00, 0x90, 0x60, 0x91, 0x00,
    0x92, 0x7F, 0x93, 0x00, 0x94, 0x00, 0x95, 0x60, 0x96, 0x00, 0x97, 0x44,
    0x98, 0x6C, 0x99, 0x00, 0x9A, 0x00, 0x9B, 0x5D, 0x9C, 0x20, 0x9D, 0x60,
    0x9F, 0x60, 0xA0, 0x00, 0xA1, 0x00, 0xA2, 0x00, 0xA3, 0x50, 0xA4, 0x00,
    0xA5, 0x00, 0xA6, 0x00, 0xA7, 0x00, 0xA8, 0x00, 0xA9, 0x00, 0xAA, 0x29,
    0xAB, 0x00, 0xAC, 0x20, 0xAD, 0x20, 0xAE, 0x00, 0xAF, 0x40, 0xF1, 0xB5,
    0x00, 0xF9, 0x80, 0x20, 0x81, 0x60, 0x82, 0x00, 0x83, 0x00, 0x84, 0x00,
    0x85, 0x60, 0x86, 0x20, 0x87, 0x00, 0x88, 0x76, 0x89, 0x00, 0x8A, 0x00,
    0x8B, 0x00, 0x8F, 0x40, 0x90, 0x60, 0x91, 0x52, 0x92, 0x42, 0x93, 0x7F,
    0x94, 0x60, 0x95, 0x60, 0x96, 0x00, 0x97, 0x2C, 0x98, 0x1B, 0x99, 0x00,
    0x9A, 0x14, 0x9B, 0x62, 0x9C, 0x40, 0x9D, 0x60, 0x9F, 0x20, 0xA0, 0x20,
    0xA1, 0x20, 0xA2, 0x00, 0xA3, 0x67, 0xA4, 0x00, 0xA5, 0x40, 0xA6, 0x30,
    0xA7, 0x50, 0xA8, 0x20, 0xA9, 0x00, 0xAA, 0x4F, 0xAB, 0x00, 0xAC, 0x29,
    0xAD, 0x20, 0xAE, 0x7F, 0xAF, 0x40, 0xF9, 0xB5, 0x00, 0xF1, 0x9E, 0x5C,
    0xF9, 0x9E, 0x74, 0xF4, 0xB0, 0x01, 0xF4, 0xB7, 0x00, 0xF4, 0xB8, 0x00,
    0xB9, 0x00, 0xBA, 0x00, 0x8C, 0x00, 0x8D, 0x00, 0x8E, 0x00, 0xB0, 0x01,
    0xF1, 0xB6, 0x7F, 0xF9, 0xB6, 0x7F, 0xF1, 0xBB, 0x00, 0xF9, 0xBB, 0x00,
    0xF1, 0xBD, 0x00, 0xB4, 0x51, 0xBE, 0x51, 0xF9, 0xBD, 0x00, 0xB4, 0x32,
    0xBE, 0x32, 0xF9, 0xB4, 0x32, 0xBE, 0x32, 0xF1, 0xB4, 0x51, 0xBE, 0x51,
    0xF9, 0xB5, 0x00, 0xF1, 0xB5, 0x00};

static unsigned char initUData[] = {0xF9, 0xF9, 0xF9, 0xF9,
                                    0xF9, 0xF9, 0xF9, 0xF9};
static unsigned char initLData[] = {0xF1, 0xF1, 0xF1, 0xF1,
                                    0xF1, 0xF1, 0xF1, 0xF1};
static unsigned char noteOnData[] = {0xF9, 0xC0, 0x48, 0x74,
                                     0xF1, 0xC0, 0x48, 0x74};
static unsigned char noteOffData[] = {0xF9, 0xD0, 0x48, 0x74,
                                      0xF1, 0xD0, 0x48, 0x74};
static unsigned char cChangeData[] = {0xF4, 0xD0, 0x48};
static unsigned char paramChangeData[] = {0xF9, 0x80, 0x00};

SynthEngine::SynthEngine() {
  int currentBoard = -1;
  int currentParam = -1;
  for (size_t i = 0; i < sizeof(pianoPatchData); i++) {
    unsigned char val = pianoPatchData[i];
    if (val >= 0xF0) {
      currentBoard = val;
      currentParam = -1;
    } else if (val >= 0x80) {
      currentParam = val;
    } else if (currentParam != -1 && currentParam <= 0xFF &&
               currentBoard != -1) {
      if (currentBoard == SYNTH_SELECT_BOTH) {
        toneU[currentParam] = val;
        toneL[currentParam] = val;
      } else {
        auto tone = currentBoard == SYNTH_SELECT_UPPER ? toneU : toneL;
        tone[currentParam] = val;
      }
      currentParam = -1;
    }
  }
}

SynthEngine::~SynthEngine() {
  running = false;
  if (synthThreadPtr.get() != nullptr) {
    synthThreadPtr->join();
  }
  if (serialPort.get() != nullptr) {
      serialPort->close();
  }
  Pm_Terminate();
}

bool SynthEngine::init(const serial::PortInfo &port) {
  if (!initSerial(port)) {
    return false;
  }

  initPortMidi();

  // Starts the synth loop thread
  running = true;
  synthThreadPtr = std::make_unique<std::thread>([this] { threadStart(); });

  return true;
}

bool SynthEngine::initSerial(const serial::PortInfo &port) {
  serialPort = std::make_unique<serial::Serial>(port.port, 31250);

  // Send init data to the synth
  serialPort->write(initLData, sizeof(initLData));
  serialPort->write(initUData, sizeof(initUData));
  serialPort->write(pianoPatchData, sizeof(pianoPatchData));

  return true;
}

bool SynthEngine::initPortMidi() {
  Pm_Initialize();

  int in_id = Pm_CreateVirtualInput("MKS-70", NULL, NULL);

  Pm_OpenInput(&midiInStream, in_id, NULL, 0, NULL, NULL);
  Pm_SetFilter(midiInStream, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);

  // Empty the buffer, just in case anything got through
  PmEvent receiveBuffer[1];
  while (Pm_Poll(midiInStream)) {
    Pm_Read(midiInStream, receiveBuffer, 1);
  }

  return true;
}

void SynthEngine::threadStart() {
  PmEvent event;

  while (running) {
    // Read from virtual port
    int length = Pm_Read(midiInStream, &event, 1);
    if (length == 0) {
      continue;
    }

    if (Pm_MessageStatus(event.message) == 0x90) {
      // piano.down(Pm_MessageData1(event.message),
      // Pm_MessageData2(event.message));
      noteOn(Pm_MessageData1(event.message), Pm_MessageData2(event.message));
    } else if (Pm_MessageStatus(event.message) == 0x80) {
      // piano.up(Pm_MessageData1(event.message));
      noteOff(Pm_MessageData1(event.message), Pm_MessageData2(event.message));
    } else if (Pm_MessageStatus(event.message) == 0xB0) {
      int controlChange = Pm_MessageData1(event.message);
      if (controlChange == 0x40)
        controlChange = 0xBB; // Sustain
      else if (controlChange == 0x01)
        controlChange = 0xBC; // Modulation

      cChangeData[1] = controlChange;
      cChangeData[2] = Pm_MessageData2(event.message);

      std::scoped_lock lock(serialFdMutex);
      if (lastBoardSelected == 0xF4) {
        serialPort->write(cChangeData + 1, sizeof(cChangeData) - 1);
      } else {
        serialPort->write(cChangeData, sizeof(cChangeData));
      }
      lastBoardSelected = 0xF4;
    } else if (Pm_MessageStatus(event.message) == 0xE0) {
      std::scoped_lock lock(serialFdMutex);
      // Bend value
      cChangeData[1] = 0xB2;
      cChangeData[2] = abs((int)Pm_MessageData2(event.message) - 0x40) * 2;

      if (lastBoardSelected == 0xF4) {
        serialPort->write(cChangeData + 1, sizeof(cChangeData) - 1);
      } else {
        serialPort->write(cChangeData, sizeof(cChangeData));
      }
      lastBoardSelected = 0xF4;

      // Bend polarity
      cChangeData[1] = 0xBF;
      cChangeData[2] = Pm_MessageData2(event.message) > 0x40 ? 0x7F : 0x00;
      serialPort->write(cChangeData + 1, sizeof(cChangeData) - 1);
    }
  }
}

void SynthEngine::changeParam(bool upper, int param, int value) {
  paramChangeData[0] = upper ? SYNTH_SELECT_UPPER : SYNTH_SELECT_LOWER;
  paramChangeData[1] = param;
  paramChangeData[2] = value;
  std::scoped_lock lock(serialFdMutex);
  serialPort->write(paramChangeData, sizeof(paramChangeData));
}

void SynthEngine::noteOn(int note, int velocity) {
  int freeVoiceIU = getBestNewVoiceId(true);
  int freeVoiceIL = getBestNewVoiceId(false);

  claimVoice(freeVoiceIU, true, note, velocity);
  claimVoice(freeVoiceIL, false, note, velocity);

  noteOnData[1] = 0xC0 | (freeVoiceIL & 0x0F);
  noteOnData[5] = 0xC0 | (freeVoiceIU & 0x0F);

  noteOnData[2] = note;
  noteOnData[6] = note;
  noteOnData[3] = velocity;
  noteOnData[7] = velocity;

  std::scoped_lock lock(serialFdMutex);
  serialPort->write(noteOnData, sizeof(noteOnData));
  lastBoardSelected = 0x00;
}

void SynthEngine::noteOff(int note, int velocity) {
  int foundVoiceIL = -1;
  int foundVoiceIU = -1;
  for (size_t j = 0; j < 6; j++) {
    if (voicesStateL[j].noteId == note) {
      foundVoiceIL = j;
      break;
    }
  }
  for (size_t j = 0; j < 6; j++) {
    if (voicesStateU[j].noteId == note) {
      foundVoiceIU = j;
      break;
    }
  }

  if (foundVoiceIL != -1 && foundVoiceIU != -1) {
    voicesStateL[foundVoiceIL].on = false;
    voicesStateU[foundVoiceIU].on = false;

    noteOffData[1] = 0xD0 | (foundVoiceIL & 0x0F);
    noteOffData[5] = 0xD0 | (foundVoiceIU & 0x0F);

    noteOffData[2] = note;
    noteOffData[6] = note;

    noteOffData[3] = velocity;
    noteOffData[7] = velocity;

    std::scoped_lock lock(serialFdMutex);
    serialPort->write(noteOffData, sizeof(noteOffData));
    lastBoardSelected = 0x00;
  }
}

int SynthEngine::getBestNewVoiceId(bool upper) {
  auto voiceState = upper ? voicesStateU : voicesStateL;

  for (size_t i = 0; i < SYNTH_NVOICES; i++) {
    if (!voiceState[i].on) {
      return i;
    }
  }

  // Not found, steal one
  std::time_t min = std::time(0);
  int iWithMin = 0;
  for (size_t i = 0; i < SYNTH_NVOICES; i++) {
    if (voiceState[i].timestamp < min) {
      iWithMin = i;
      min = voiceState[i].timestamp;
    }
  }
  return iWithMin;
}

void SynthEngine::claimVoice(int voiceId, bool upper, int note, int velocity) {
  auto voiceState = upper ? voicesStateU : voicesStateL;
  voiceState[voiceId].on = true;
  voiceState[voiceId].timestamp = std::time(0);
  voiceState[voiceId].noteId = note;
  voiceState[voiceId].velocity = velocity;
}
