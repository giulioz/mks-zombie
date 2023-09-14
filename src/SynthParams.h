#ifndef SYNTHPARAMS_H
#define SYNTHPARAMS_H

#include <chrono>

enum ToneParamTable {
  DCO1_RAN = 0x80,
  DCO1_WF = 0x81,
  DCO1_TUN = 0x82,
  DCO1_LFO = 0x83,
  DCO1_ENV = 0x84,
  DCO2_RAN = 0x85,
  DCO2_WF = 0x86,
  XMOD = 0x87,
  DCO2_TUN = 0x88,
  DCO2_FTU = 0x89,
  DCO2_LFO = 0x8A,
  DCO2_ENV = 0x8B,
  ATOUCH_VIB = 0x8C,
  ATOUCH_BRI = 0x8D,
  ATOUCH_VOL = 0x8E,
  DCO_DYNA = 0x8F,
  DCO_MODE = 0x90,
  MIX_DCO1 = 0x91,
  MIX_DCO2 = 0x92,
  MIX_ENV = 0x93,
  MIX_DYNA = 0x94,
  MIX_MODE = 0x95,
  HPF_FREQ = 0x96,
  VCF_FREQ = 0x97,
  VCF_RES = 0x98,
  VCF_LFO = 0x99,
  VCF_ENV = 0x9A,
  VCF_KEY = 0x9B,
  VCF_DYNA = 0x9C,
  VCF_MODE = 0x9D,
  VCA_LEVE = 0x9E,
  VCA_DYNA = 0x9F,
  CHORUS = 0xA0,
  LFO_WF = 0xA1,
  LFO_DELA = 0xA2,
  LFO_RATE = 0xA3,
  ENV1_ATT = 0xA4,
  ENV1_DEC = 0xA5,
  ENV1_SUS = 0xA6,
  ENV1_REL = 0xA7,
  ENV1_KEY = 0xA8,
  ENV2_ATT = 0xA9,
  ENV2_DEC = 0xAA,
  ENV2_SUS = 0xAB,
  ENV2_REL = 0xAC,
  ENV2_KEY = 0xAD,
  UNK_0xAE = 0xAE,
  VCA_MODE = 0xAF,
};

struct VoiceState {
  bool on = false;
  std::time_t timestamp = 0;
  int noteId;
  int velocity;
};

enum PolyMode { Poly1, Poly2, Unison1, Unison2, Mono1, Mono2 };
enum PatchMode { Whole, Dual };

#endif
