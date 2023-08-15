#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <IOKit/serial/ioss.h>
#include <SDL.h>
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <portmidi.h>
#include <porttime.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "Piano.h"

#define OUTPUT_BUFFER_SIZE 0
#define INPUT_BUFFER_SIZE 10
#define TIME_PROC ((PmTimeProcPtr)Pt_Time)
#define TIME_INFO NULL

unsigned char pianoPatch[] = {
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

unsigned char initU[] = {0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9};
unsigned char initL[] = {0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1};
unsigned char noteOn[] = {0xF9, 0xC0, 0x48, 0x74, 0xF1, 0xC0, 0x48, 0x74};
unsigned char noteOff[] = {0xF9, 0xD0, 0x48, 0x74, 0xF1, 0xD0, 0x48, 0x74};
unsigned char cChange[] = {0xF4, 0xD0, 0x48};
unsigned char paramChange[] = {0xF9, 0x80, 0x00};

struct Tone {
  int DCO1_RAN;
  int DCO1_WF;
  int DCO1_TUN;
  int DCO1_LFO;
  int DCO1_ENV;
  int DCO2_RAN;
  int DCO2_WF;
  int XMOD;
  int DCO2_TUN;
  int DCO2_FTU;
  int DCO2_LFO;
  int DCO2_ENV;
  int ATOUCH_VIB;
  int ATOUCH_BRI;
  int ATOUCH_VOL;
  int DCO_DYNA;
  int DCO_MODE;
  int MIX_DCO1;
  int MIX_DCO2;
  int MIX_ENV;
  int MIX_DYNA;
  int MIX_MODE;
  int HPF_FREQ;
  int VCF_FREQ;
  int VCF_RES;
  int VCF_LFO;
  int VCF_ENV;
  int VCF_KEY;
  int VCF_DYNA;
  int VCF_MODE;
  int VCA_LEVE;
  int VCA_DYNA;
  int CHORUS;
  int LFO_WF;
  int LFO_DELA;
  int LFO_RATE;
  int ENV1_ATT;
  int ENV1_DEC;
  int ENV1_SUS;
  int ENV1_REL;
  int ENV1_KEY;
  int ENV2_ATT;
  int ENV2_DEC;
  int ENV2_SUS;
  int ENV2_REL;
  int ENV2_KEY;
  int UNK_0xAE;
  int VCA_MODE;
};

Tone toneU = {0};
Tone toneL = {0};

struct VoiceState {
  bool on = false;
  std::time_t timestamp;
  int noteId;
  int velocity;
};
VoiceState voicesStateU[6];
VoiceState voicesStateL[6];

int lastBoardSelected = 0x00;

bool done = false;
int fd;
std::mutex fdMutex;

Piano piano;

void synthLoop();
void paramSlider(const char *name, int *ptr, unsigned char id, int board);

int main(int argc, char *argv[]) {
  fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    printf("Device cannot be opened.\n");
    exit(-1);
  }

  std::thread synthThread(synthLoop);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return 1;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                        SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window *window = SDL_CreateWindow(
      "Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  while (!done) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application, or clear/overwrite your copy of the
    // keyboard data. Generally you may always pass all inputs to dear imgui,
    // and hide them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
      if (event.type == SDL_WINDOWEVENT &&
          event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    piano.draw();

    {
      ImGui::Begin("Params U");
      paramSlider("DCO1_RAN", &toneU.DCO1_RAN, 0x80, 0xF9);
      paramSlider("DCO1_WF", &toneU.DCO1_WF, 0x81, 0xF9);
      paramSlider("DCO1_TUN", &toneU.DCO1_TUN, 0x82, 0xF9);
      paramSlider("DCO1_LFO", &toneU.DCO1_LFO, 0x83, 0xF9);
      paramSlider("DCO1_ENV", &toneU.DCO1_ENV, 0x84, 0xF9);
      paramSlider("DCO2_RAN", &toneU.DCO2_RAN, 0x85, 0xF9);
      paramSlider("DCO2_WF", &toneU.DCO2_WF, 0x86, 0xF9);
      paramSlider("XMOD", &toneU.XMOD, 0x87, 0xF9);
      paramSlider("DCO2_TUN", &toneU.DCO2_TUN, 0x88, 0xF9);
      paramSlider("DCO2_FTU", &toneU.DCO2_FTU, 0x89, 0xF9);
      paramSlider("DCO2_LFO", &toneU.DCO2_LFO, 0x8A, 0xF9);
      paramSlider("DCO2_ENV", &toneU.DCO2_ENV, 0x8B, 0xF9);
      paramSlider("ATOUCH_VIB", &toneU.ATOUCH_VIB, 0x8C, 0xF9);
      paramSlider("ATOUCH_BRI", &toneU.ATOUCH_BRI, 0x8D, 0xF9);
      paramSlider("ATOUCH_VOL", &toneU.ATOUCH_VOL, 0x8E, 0xF9);
      paramSlider("DCO_DYNA", &toneU.DCO_DYNA, 0x8F, 0xF9);
      paramSlider("DCO_MODE", &toneU.DCO_MODE, 0x90, 0xF9);
      paramSlider("MIX_DCO1", &toneU.MIX_DCO1, 0x91, 0xF9);
      paramSlider("MIX_DCO2", &toneU.MIX_DCO2, 0x92, 0xF9);
      paramSlider("MIX_ENV", &toneU.MIX_ENV, 0x93, 0xF9);
      paramSlider("MIX_DYNA", &toneU.MIX_DYNA, 0x94, 0xF9);
      paramSlider("MIX_MODE", &toneU.MIX_MODE, 0x95, 0xF9);
      paramSlider("HPF_FREQ", &toneU.HPF_FREQ, 0x96, 0xF9);
      paramSlider("VCF_FREQ", &toneU.VCF_FREQ, 0x97, 0xF9);
      paramSlider("VCF_RES", &toneU.VCF_RES, 0x98, 0xF9);
      paramSlider("VCF_LFO", &toneU.VCF_LFO, 0x99, 0xF9);
      paramSlider("VCF_ENV", &toneU.VCF_ENV, 0x9A, 0xF9);
      paramSlider("VCF_KEY", &toneU.VCF_KEY, 0x9B, 0xF9);
      paramSlider("VCF_DYNA", &toneU.VCF_DYNA, 0x9C, 0xF9);
      paramSlider("VCF_MODE", &toneU.VCF_MODE, 0x9D, 0xF9);
      paramSlider("VCA_LEVE", &toneU.VCA_LEVE, 0x9E, 0xF9);
      paramSlider("VCA_DYNA", &toneU.VCA_DYNA, 0x9F, 0xF9);
      paramSlider("CHORUS", &toneU.CHORUS, 0xA0, 0xF9);
      paramSlider("LFO_WF", &toneU.LFO_WF, 0xA1, 0xF9);
      paramSlider("LFO_DELA", &toneU.LFO_DELA, 0xA2, 0xF9);
      paramSlider("LFO_RATE", &toneU.LFO_RATE, 0xA3, 0xF9);
      paramSlider("ENV1_ATT", &toneU.ENV1_ATT, 0xA4, 0xF9);
      paramSlider("ENV1_DEC", &toneU.ENV1_DEC, 0xA5, 0xF9);
      paramSlider("ENV1_SUS", &toneU.ENV1_SUS, 0xA6, 0xF9);
      paramSlider("ENV1_REL", &toneU.ENV1_REL, 0xA7, 0xF9);
      paramSlider("ENV1_KEY", &toneU.ENV1_KEY, 0xA8, 0xF9);
      paramSlider("ENV2_ATT", &toneU.ENV2_ATT, 0xA9, 0xF9);
      paramSlider("ENV2_DEC", &toneU.ENV2_DEC, 0xAA, 0xF9);
      paramSlider("ENV2_SUS", &toneU.ENV2_SUS, 0xAB, 0xF9);
      paramSlider("ENV2_REL", &toneU.ENV2_REL, 0xAC, 0xF9);
      paramSlider("ENV2_KEY", &toneU.ENV2_KEY, 0xAD, 0xF9);
      paramSlider("UNK_0xAE", &toneU.UNK_0xAE, 0xAE, 0xF9);
      paramSlider("VCA_MODE", &toneU.VCA_MODE, 0xAF, 0xF9);
      ImGui::End();
    }
    {
      ImGui::Begin("Params L");
      paramSlider("DCO1_RAN", &toneL.DCO1_RAN, 0x80, 0xF1);
      paramSlider("DCO1_WF", &toneL.DCO1_WF, 0x81, 0xF1);
      paramSlider("DCO1_TUN", &toneL.DCO1_TUN, 0x82, 0xF1);
      paramSlider("DCO1_LFO", &toneL.DCO1_LFO, 0x83, 0xF1);
      paramSlider("DCO1_ENV", &toneL.DCO1_ENV, 0x84, 0xF1);
      paramSlider("DCO2_RAN", &toneL.DCO2_RAN, 0x85, 0xF1);
      paramSlider("DCO2_WF", &toneL.DCO2_WF, 0x86, 0xF1);
      paramSlider("XMOD", &toneL.XMOD, 0x87, 0xF1);
      paramSlider("DCO2_TUN", &toneL.DCO2_TUN, 0x88, 0xF1);
      paramSlider("DCO2_FTU", &toneL.DCO2_FTU, 0x89, 0xF1);
      paramSlider("DCO2_LFO", &toneL.DCO2_LFO, 0x8A, 0xF1);
      paramSlider("DCO2_ENV", &toneL.DCO2_ENV, 0x8B, 0xF1);
      paramSlider("ATOUCH_VIB", &toneL.ATOUCH_VIB, 0x8C, 0xF1);
      paramSlider("ATOUCH_BRI", &toneL.ATOUCH_BRI, 0x8D, 0xF1);
      paramSlider("ATOUCH_VOL", &toneL.ATOUCH_VOL, 0x8E, 0xF1);
      paramSlider("DCO_DYNA", &toneL.DCO_DYNA, 0x8F, 0xF1);
      paramSlider("DCO_MODE", &toneL.DCO_MODE, 0x90, 0xF1);
      paramSlider("MIX_DCO1", &toneL.MIX_DCO1, 0x91, 0xF1);
      paramSlider("MIX_DCO2", &toneL.MIX_DCO2, 0x92, 0xF1);
      paramSlider("MIX_ENV", &toneL.MIX_ENV, 0x93, 0xF1);
      paramSlider("MIX_DYNA", &toneL.MIX_DYNA, 0x94, 0xF1);
      paramSlider("MIX_MODE", &toneL.MIX_MODE, 0x95, 0xF1);
      paramSlider("HPF_FREQ", &toneL.HPF_FREQ, 0x96, 0xF1);
      paramSlider("VCF_FREQ", &toneL.VCF_FREQ, 0x97, 0xF1);
      paramSlider("VCF_RES", &toneL.VCF_RES, 0x98, 0xF1);
      paramSlider("VCF_LFO", &toneL.VCF_LFO, 0x99, 0xF1);
      paramSlider("VCF_ENV", &toneL.VCF_ENV, 0x9A, 0xF1);
      paramSlider("VCF_KEY", &toneL.VCF_KEY, 0x9B, 0xF1);
      paramSlider("VCF_DYNA", &toneL.VCF_DYNA, 0x9C, 0xF1);
      paramSlider("VCF_MODE", &toneL.VCF_MODE, 0x9D, 0xF1);
      paramSlider("VCA_LEVE", &toneL.VCA_LEVE, 0x9E, 0xF1);
      paramSlider("VCA_DYNA", &toneL.VCA_DYNA, 0x9F, 0xF1);
      paramSlider("CHORUS", &toneL.CHORUS, 0xA0, 0xF1);
      paramSlider("LFO_WF", &toneL.LFO_WF, 0xA1, 0xF1);
      paramSlider("LFO_DELA", &toneL.LFO_DELA, 0xA2, 0xF1);
      paramSlider("LFO_RATE", &toneL.LFO_RATE, 0xA3, 0xF1);
      paramSlider("ENV1_ATT", &toneL.ENV1_ATT, 0xA4, 0xF1);
      paramSlider("ENV1_DEC", &toneL.ENV1_DEC, 0xA5, 0xF1);
      paramSlider("ENV1_SUS", &toneL.ENV1_SUS, 0xA6, 0xF1);
      paramSlider("ENV1_REL", &toneL.ENV1_REL, 0xA7, 0xF1);
      paramSlider("ENV1_KEY", &toneL.ENV1_KEY, 0xA8, 0xF1);
      paramSlider("ENV2_ATT", &toneL.ENV2_ATT, 0xA9, 0xF1);
      paramSlider("ENV2_DEC", &toneL.ENV2_DEC, 0xAA, 0xF1);
      paramSlider("ENV2_SUS", &toneL.ENV2_SUS, 0xAB, 0xF1);
      paramSlider("ENV2_REL", &toneL.ENV2_REL, 0xAC, 0xF1);
      paramSlider("ENV2_KEY", &toneL.ENV2_KEY, 0xAD, 0xF1);
      paramSlider("UNK_0xAE", &toneL.UNK_0xAE, 0xAE, 0xF1);
      paramSlider("VCA_MODE", &toneL.VCA_MODE, 0xAF, 0xF1);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }

  synthThread.join();

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  Pm_Terminate();

  return 0;
}

void paramSlider(const char *name, int *ptr, unsigned char id, int board) {
  bool changed = ImGui::SliderInt(name, ptr, 0, 127);
  if (changed) {
    paramChange[0] = board;
    paramChange[1] = id;
    paramChange[2] = *ptr;
    std::scoped_lock lock(fdMutex);
    write(fd, paramChange, sizeof(paramChange));
  }
}

void synthLoop() {
  struct termios options;
  fcntl(fd, F_SETFL, FNDELAY);
  tcgetattr(fd, &options);          // Get the current options of the port
  bzero(&options, sizeof(options)); // Clear all the options
  // Configure the device : 8 bits, no parity, no control
  options.c_cflag |= (CLOCAL | CREAD | CS8);
  options.c_iflag |= (IGNPAR | IGNBRK);
  options.c_cc[VTIME] = 0; // Timer unused
  options.c_cc[VMIN] = 0;  // At least on character before satisfy reading
  tcsetattr(fd, TCSANOW, &options);

  speed_t baud = 31250;
  // speed_t baud = 38400;
  ioctl(fd, IOSSIOSPEED, &baud);

  write(fd, initL, sizeof(initL));
  write(fd, initU, sizeof(initU));
  write(fd, pianoPatch, sizeof(pianoPatch));

  Pm_Initialize();

  int in_id = Pm_CreateVirtualInput("MKS-70", NULL, NULL);
  int out_id = Pm_CreateVirtualOutput("MKS-70", NULL, NULL);

  PmEvent buffer[1];
  PmStream *out;
  PmStream *in;
  Pm_OpenInput(&in, in_id, NULL, 0, NULL, NULL);
  Pm_OpenOutput(&out, out_id, NULL, OUTPUT_BUFFER_SIZE, TIME_PROC, TIME_INFO,
                0);
  printf("Created/Opened input %d and output %d\n", in_id, out_id);
  Pm_SetFilter(in, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);

  // Empty the buffer, just in case anything got through
  while (Pm_Poll(in)) {
    Pm_Read(in, buffer, 1);
  }

  while (!done) {
    // Read from virtual port
    int length = Pm_Read(in, buffer, 1);
    if (length > 0) {
      for (size_t i = 0; i < length; i++) {
        // printf("Got message: time %ld, %2lx %2lx %2lx\n",
        //        (long)buffer[i].timestamp,
        //        (long)Pm_MessageStatus(buffer[i].message),
        //        (long)Pm_MessageData1(buffer[i].message),
        //        (long)Pm_MessageData2(buffer[i].message));

        if (Pm_MessageStatus(buffer[i].message) == 0x90) {
          piano.down(Pm_MessageData1(buffer[i].message),
                     Pm_MessageData2(buffer[i].message));

          int freeVoiceIL = -1;
          int freeVoiceIU = -1;
          for (size_t j = 0; j < 6; j++) {
            if (!voicesStateL[j].on) {
              freeVoiceIL = j;
              break;
            }
          }
          for (size_t j = 0; j < 6; j++) {
            if (!voicesStateU[j].on) {
              freeVoiceIU = j;
              break;
            }
          }
          if (freeVoiceIL == -1 || freeVoiceIU == -1) {
            freeVoiceIL = 0;
            freeVoiceIU = 0;
          }

          voicesStateL[freeVoiceIL].on = true;
          voicesStateL[freeVoiceIL].timestamp = std::time(0);
          voicesStateL[freeVoiceIL].noteId = Pm_MessageData1(buffer[i].message);
          voicesStateL[freeVoiceIL].velocity =
              Pm_MessageData2(buffer[i].message);
          voicesStateU[freeVoiceIU].on = true;
          voicesStateU[freeVoiceIU].timestamp = std::time(0);
          voicesStateU[freeVoiceIU].noteId = Pm_MessageData1(buffer[i].message);
          voicesStateU[freeVoiceIU].velocity =
              Pm_MessageData2(buffer[i].message);

          noteOn[1] = 0xC0 | (freeVoiceIL & 0x0F);
          noteOn[5] = 0xC0 | (freeVoiceIU & 0x0F);

          noteOn[2] = Pm_MessageData1(buffer[i].message);
          noteOn[6] = Pm_MessageData1(buffer[i].message);
          noteOn[3] = Pm_MessageData2(buffer[i].message);
          noteOn[7] = Pm_MessageData2(buffer[i].message);

          std::scoped_lock lock(fdMutex);
          write(fd, noteOn, sizeof(noteOn));
          lastBoardSelected = 0x00;
        } else if (Pm_MessageStatus(buffer[i].message) == 0x80) {
          piano.up(Pm_MessageData1(buffer[i].message));

          int foundVoiceIL = -1;
          int foundVoiceIU = -1;
          for (size_t j = 0; j < 6; j++) {
            if (voicesStateL[j].noteId == Pm_MessageData1(buffer[i].message)) {
              foundVoiceIL = j;
              break;
            }
          }
          for (size_t j = 0; j < 6; j++) {
            if (voicesStateU[j].noteId == Pm_MessageData1(buffer[i].message)) {
              foundVoiceIU = j;
              break;
            }
          }

          if (foundVoiceIL != -1 && foundVoiceIU != -1) {
            voicesStateL[foundVoiceIL].on = false;
            voicesStateU[foundVoiceIU].on = false;

            noteOff[1] = 0xD0 | (foundVoiceIL & 0x0F);
            noteOff[5] = 0xD0 | (foundVoiceIU & 0x0F);

            noteOff[2] = Pm_MessageData1(buffer[i].message);
            noteOff[6] = Pm_MessageData1(buffer[i].message);

            noteOff[3] = Pm_MessageData2(buffer[i].message);
            noteOff[7] = Pm_MessageData2(buffer[i].message);

            std::scoped_lock lock(fdMutex);
            write(fd, noteOff, sizeof(noteOff));
            lastBoardSelected = 0x00;
          }
        } else if (Pm_MessageStatus(buffer[i].message) == 0xB0) {
          int controlChange = Pm_MessageData1(buffer[i].message);
          if (controlChange == 0x40)
            controlChange = 0xBB; // Sustain
          else if (controlChange == 0x01)
            controlChange = 0xBC; // Modulation

          cChange[1] = controlChange;
          cChange[2] = Pm_MessageData2(buffer[i].message);

          std::scoped_lock lock(fdMutex);
          if (lastBoardSelected == 0xF4) {
            write(fd, cChange + 1, sizeof(cChange) - 1);
          } else {
            write(fd, cChange, sizeof(cChange));
          }
          lastBoardSelected = 0xF4;
        } else if (Pm_MessageStatus(buffer[i].message) == 0xE0) {
          std::scoped_lock lock(fdMutex);
          // Bend value
          cChange[1] = 0xB2;
          cChange[2] = abs((int)Pm_MessageData2(buffer[i].message) - 0x40) * 2;

          if (lastBoardSelected == 0xF4) {
            write(fd, cChange + 1, sizeof(cChange) - 1);
          } else {
            write(fd, cChange, sizeof(cChange));
          }
          lastBoardSelected = 0xF4;

          // Bend polarity
          cChange[1] = 0xBF;
          cChange[2] = Pm_MessageData2(buffer[i].message) > 0x40 ? 0x7F : 0x00;
          write(fd, cChange + 1, sizeof(cChange) - 1);
        }
      }
    }
  }
}
