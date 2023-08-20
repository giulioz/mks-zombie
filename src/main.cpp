#include "hello_imgui/hello_imgui.h"
#include "serial/serial.h"
#include <chrono>
#include <iostream>
#include <nfd.h>
#include <stdio.h>
#include <string>

#include "Piano.h"
#include "SynthEngine.h"
#include "SynthParams.h"

bool done = false;
SynthEngine synthEngine;

Piano piano;

void paramSlider(const char *name, int *ptr, unsigned char id, bool upper) {
  std::string concatName = name;
  concatName.append(upper ? "##UPPER" : "##LOWER");
  bool changed = ImGui::SliderInt(concatName.c_str(), ptr, 0, 127);
  if (changed) {
    synthEngine.changeParam(upper, id, *ptr);
  }
}

void toneParams(int dest[], bool upper) {
  ImGui::Text("DCO1");
  paramSlider("DCO1_RAN", &dest[0x80], 0x80, upper);
  paramSlider("DCO1_WF", &dest[0x81], 0x81, upper);
  paramSlider("DCO1_TUN", &dest[0x82], 0x82, upper);
  paramSlider("DCO1_LFO", &dest[0x83], 0x83, upper);
  paramSlider("DCO1_ENV", &dest[0x84], 0x84, upper);

  ImGui::Text("DCO2");
  paramSlider("DCO2_RAN", &dest[0x85], 0x85, upper);
  paramSlider("DCO2_WF", &dest[0x86], 0x86, upper);
  paramSlider("XMOD", &dest[0x87], 0x87, upper);
  paramSlider("DCO2_TUN", &dest[0x88], 0x88, upper);
  paramSlider("DCO2_FTU", &dest[0x89], 0x89, upper);
  paramSlider("DCO2_LFO", &dest[0x8A], 0x8A, upper);
  paramSlider("DCO2_ENV", &dest[0x8B], 0x8B, upper);

  ImGui::Text("Aftertouch");
  paramSlider("ATOUCH_VIB", &dest[0x8C], 0x8C, upper);
  paramSlider("ATOUCH_BRI", &dest[0x8D], 0x8D, upper);
  paramSlider("ATOUCH_VOL", &dest[0x8E], 0x8E, upper);

  ImGui::Text("Mix");
  paramSlider("DCO_DYNA", &dest[0x8F], 0x8F, upper);
  paramSlider("DCO_MODE", &dest[0x90], 0x90, upper);
  paramSlider("MIX_DCO1", &dest[0x91], 0x91, upper);
  paramSlider("MIX_DCO2", &dest[0x92], 0x92, upper);
  paramSlider("MIX_ENV", &dest[0x93], 0x93, upper);
  paramSlider("MIX_DYNA", &dest[0x94], 0x94, upper);
  paramSlider("MIX_MODE", &dest[0x95], 0x95, upper);

  ImGui::Text("VCF");
  paramSlider("HPF_FREQ", &dest[0x96], 0x96, upper);
  paramSlider("VCF_FREQ", &dest[0x97], 0x97, upper);
  paramSlider("VCF_RES", &dest[0x98], 0x98, upper);
  paramSlider("VCF_LFO", &dest[0x99], 0x99, upper);
  paramSlider("VCF_ENV", &dest[0x9A], 0x9A, upper);
  paramSlider("VCF_KEY", &dest[0x9B], 0x9B, upper);
  paramSlider("VCF_DYNA", &dest[0x9C], 0x9C, upper);
  paramSlider("VCF_MODE", &dest[0x9D], 0x9D, upper);

  ImGui::Text("VCA");
  paramSlider("VCA_LEVE", &dest[0x9E], 0x9E, upper);
  paramSlider("VCA_DYNA", &dest[0x9F], 0x9F, upper);
  paramSlider("VCA_MODE", &dest[0xAF], 0xAF, upper);
  paramSlider("CHORUS", &dest[0xA0], 0xA0, upper);

  ImGui::Text("LFO");
  paramSlider("LFO_WF", &dest[0xA1], 0xA1, upper);
  paramSlider("LFO_DELA", &dest[0xA2], 0xA2, upper);
  paramSlider("LFO_RATE", &dest[0xA3], 0xA3, upper);

  ImGui::Text("ENV1");
  paramSlider("ENV1_ATT", &dest[0xA4], 0xA4, upper);
  paramSlider("ENV1_DEC", &dest[0xA5], 0xA5, upper);
  paramSlider("ENV1_SUS", &dest[0xA6], 0xA6, upper);
  paramSlider("ENV1_REL", &dest[0xA7], 0xA7, upper);
  paramSlider("ENV1_KEY", &dest[0xA8], 0xA8, upper);

  ImGui::Text("ENV2");
  paramSlider("ENV2_ATT", &dest[0xA9], 0xA9, upper);
  paramSlider("ENV2_DEC", &dest[0xAA], 0xAA, upper);
  paramSlider("ENV2_SUS", &dest[0xAB], 0xAB, upper);
  paramSlider("ENV2_REL", &dest[0xAC], 0xAC, upper);
  paramSlider("ENV2_KEY", &dest[0xAD], 0xAD, upper);

  paramSlider("UNK_0xAE", &dest[0xAE], 0xAE, upper);
}

void synthInitUI(bool *serialPortInited) {
  std::vector<serial::PortInfo> serialDevices = serial::list_ports();
  std::vector<const char *> serialDevicesNames;
  for (auto &&dev : serialDevices) {
    serialDevicesNames.push_back(dev.port.c_str());
  }

  static int selectedSerialPort = 0;

  if (!*serialPortInited) {
    ImGui::OpenPopup("Select serial port");
  }

  if (ImGui::BeginPopupModal("Select serial port", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::ListBox("##SerialPort", &selectedSerialPort,
                       serialDevicesNames.data(), serialDevices.size())) {
      synthEngine.init(serialDevices[selectedSerialPort]);
      *serialPortInited = true;

      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

int main(int argc, char *argv[]) {
  NFD_Init();

  // nfdchar_t *outPath;
  // nfdfilteritem_t filterItem[2] = {{"Source code", "c,cpp,cc"},
  //                                  {"Headers", "h,hpp"}};
  // nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
  // if (result == NFD_OKAY) {
  //   puts("Success!");
  //   puts(outPath);
  //   NFD_FreePath(outPath);
  // } else if (result == NFD_CANCEL) {
  //   puts("User pressed cancel.");
  // } else {
  //   printf("Error: %s\n", NFD_GetError());
  // }

  bool serialPortInited = false;

  HelloImGui::Run(
      [&] {
        // piano.draw();

        synthInitUI(&serialPortInited);

        ImGui::BeginTable("table1", 3);

        ImGui::TableNextColumn();
        ImGui::Text("Common");

        int tempVal = 0;
        bool tempBool = 0;
        ImGui::Combo("PATCH MODE", &tempVal, "WHOLE\0DUAL");
        ImGui::Checkbox("PORTAMENTO ENABLE", &tempBool);
        ImGui::SliderInt("PORTAMENTO TIME", &tempVal, 0, 127);
        ImGui::SliderInt("BEND RANGE", &tempVal, 0, 127);

        ImGui::Combo("POLY MODE", (int *)&synthEngine.polyMode,
                     "POLY 1\0POLY 2\0UNISON 1\0UNISON 2\0MONO 1\0MONO 2");

        ImGui::TableNextColumn();
        ImGui::Text("Upper");
        toneParams(synthEngine.toneU, true);

        ImGui::TableNextColumn();
        ImGui::Text("Lower");
        toneParams(synthEngine.toneL, false);

        ImGui::EndTable();
      },
      "MKS-70 Zombie Mode", false);

  NFD_Quit();

  return 0;
}
