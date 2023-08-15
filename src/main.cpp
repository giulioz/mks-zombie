#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <SDL.h>
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
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
#include "SynthEngine.h"
#include "SynthParams.h"

bool done = false;
SynthEngine synthEngine;

Piano piano;

void paramSlider(const char *name, int *ptr, unsigned char id, bool upper);

int main(int argc, char *argv[]) {
  if (!synthEngine.init(argv[1])) {
    printf("Failed to initialize the serial port!\n");
    exit(-1);
  }

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
      ImGui::Begin("Params", (bool *)__null,
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

      ImGui::BeginTable("table1", 2);
      ImGui::TableNextColumn();
      ImGui::Text("Upper");
      paramSlider("DCO1_RAN", &synthEngine.toneU[0x80], 0x80, true);
      paramSlider("DCO1_WF", &synthEngine.toneU[0x81], 0x81, true);
      paramSlider("DCO1_TUN", &synthEngine.toneU[0x82], 0x82, true);
      paramSlider("DCO1_LFO", &synthEngine.toneU[0x83], 0x83, true);
      paramSlider("DCO1_ENV", &synthEngine.toneU[0x84], 0x84, true);
      paramSlider("DCO2_RAN", &synthEngine.toneU[0x85], 0x85, true);
      paramSlider("DCO2_WF", &synthEngine.toneU[0x86], 0x86, true);
      paramSlider("XMOD", &synthEngine.toneU[0x87], 0x87, true);
      paramSlider("DCO2_TUN", &synthEngine.toneU[0x88], 0x88, true);
      paramSlider("DCO2_FTU", &synthEngine.toneU[0x89], 0x89, true);
      paramSlider("DCO2_LFO", &synthEngine.toneU[0x8A], 0x8A, true);
      paramSlider("DCO2_ENV", &synthEngine.toneU[0x8B], 0x8B, true);
      paramSlider("ATOUCH_VIB", &synthEngine.toneU[0x8C], 0x8C, true);
      paramSlider("ATOUCH_BRI", &synthEngine.toneU[0x8D], 0x8D, true);
      paramSlider("ATOUCH_VOL", &synthEngine.toneU[0x8E], 0x8E, true);
      paramSlider("DCO_DYNA", &synthEngine.toneU[0x8F], 0x8F, true);
      paramSlider("DCO_MODE", &synthEngine.toneU[0x90], 0x90, true);
      paramSlider("MIX_DCO1", &synthEngine.toneU[0x91], 0x91, true);
      paramSlider("MIX_DCO2", &synthEngine.toneU[0x92], 0x92, true);
      paramSlider("MIX_ENV", &synthEngine.toneU[0x93], 0x93, true);
      paramSlider("MIX_DYNA", &synthEngine.toneU[0x94], 0x94, true);
      paramSlider("MIX_MODE", &synthEngine.toneU[0x95], 0x95, true);
      paramSlider("HPF_FREQ", &synthEngine.toneU[0x96], 0x96, true);
      paramSlider("VCF_FREQ", &synthEngine.toneU[0x97], 0x97, true);
      paramSlider("VCF_RES", &synthEngine.toneU[0x98], 0x98, true);
      paramSlider("VCF_LFO", &synthEngine.toneU[0x99], 0x99, true);
      paramSlider("VCF_ENV", &synthEngine.toneU[0x9A], 0x9A, true);
      paramSlider("VCF_KEY", &synthEngine.toneU[0x9B], 0x9B, true);
      paramSlider("VCF_DYNA", &synthEngine.toneU[0x9C], 0x9C, true);
      paramSlider("VCF_MODE", &synthEngine.toneU[0x9D], 0x9D, true);
      paramSlider("VCA_LEVE", &synthEngine.toneU[0x9E], 0x9E, true);
      paramSlider("VCA_DYNA", &synthEngine.toneU[0x9F], 0x9F, true);
      paramSlider("CHORUS", &synthEngine.toneU[0xA0], 0xA0, true);
      paramSlider("LFO_WF", &synthEngine.toneU[0xA1], 0xA1, true);
      paramSlider("LFO_DELA", &synthEngine.toneU[0xA2], 0xA2, true);
      paramSlider("LFO_RATE", &synthEngine.toneU[0xA3], 0xA3, true);
      paramSlider("ENV1_ATT", &synthEngine.toneU[0xA4], 0xA4, true);
      paramSlider("ENV1_DEC", &synthEngine.toneU[0xA5], 0xA5, true);
      paramSlider("ENV1_SUS", &synthEngine.toneU[0xA6], 0xA6, true);
      paramSlider("ENV1_REL", &synthEngine.toneU[0xA7], 0xA7, true);
      paramSlider("ENV1_KEY", &synthEngine.toneU[0xA8], 0xA8, true);
      paramSlider("ENV2_ATT", &synthEngine.toneU[0xA9], 0xA9, true);
      paramSlider("ENV2_DEC", &synthEngine.toneU[0xAA], 0xAA, true);
      paramSlider("ENV2_SUS", &synthEngine.toneU[0xAB], 0xAB, true);
      paramSlider("ENV2_REL", &synthEngine.toneU[0xAC], 0xAC, true);
      paramSlider("ENV2_KEY", &synthEngine.toneU[0xAD], 0xAD, true);
      paramSlider("UNK_0xAE", &synthEngine.toneU[0xAE], 0xAE, true);
      paramSlider("VCA_MODE", &synthEngine.toneU[0xAF], 0xAF, true);

      ImGui::TableNextColumn();
      ImGui::Text("Lower");
      paramSlider("DCO1_RAN", &synthEngine.toneL[0x80], 0x80, false);
      paramSlider("DCO1_WF", &synthEngine.toneL[0x81], 0x81, false);
      paramSlider("DCO1_TUN", &synthEngine.toneL[0x82], 0x82, false);
      paramSlider("DCO1_LFO", &synthEngine.toneL[0x83], 0x83, false);
      paramSlider("DCO1_ENV", &synthEngine.toneL[0x84], 0x84, false);
      paramSlider("DCO2_RAN", &synthEngine.toneL[0x85], 0x85, false);
      paramSlider("DCO2_WF", &synthEngine.toneL[0x86], 0x86, false);
      paramSlider("XMOD", &synthEngine.toneL[0x87], 0x87, false);
      paramSlider("DCO2_TUN", &synthEngine.toneL[0x88], 0x88, false);
      paramSlider("DCO2_FTU", &synthEngine.toneL[0x89], 0x89, false);
      paramSlider("DCO2_LFO", &synthEngine.toneL[0x8A], 0x8A, false);
      paramSlider("DCO2_ENV", &synthEngine.toneL[0x8B], 0x8B, false);
      paramSlider("ATOUCH_VIB", &synthEngine.toneL[0x8C], 0x8C, false);
      paramSlider("ATOUCH_BRI", &synthEngine.toneL[0x8D], 0x8D, false);
      paramSlider("ATOUCH_VOL", &synthEngine.toneL[0x8E], 0x8E, false);
      paramSlider("DCO_DYNA", &synthEngine.toneL[0x8F], 0x8F, false);
      paramSlider("DCO_MODE", &synthEngine.toneL[0x90], 0x90, false);
      paramSlider("MIX_DCO1", &synthEngine.toneL[0x91], 0x91, false);
      paramSlider("MIX_DCO2", &synthEngine.toneL[0x92], 0x92, false);
      paramSlider("MIX_ENV", &synthEngine.toneL[0x93], 0x93, false);
      paramSlider("MIX_DYNA", &synthEngine.toneL[0x94], 0x94, false);
      paramSlider("MIX_MODE", &synthEngine.toneL[0x95], 0x95, false);
      paramSlider("HPF_FREQ", &synthEngine.toneL[0x96], 0x96, false);
      paramSlider("VCF_FREQ", &synthEngine.toneL[0x97], 0x97, false);
      paramSlider("VCF_RES", &synthEngine.toneL[0x98], 0x98, false);
      paramSlider("VCF_LFO", &synthEngine.toneL[0x99], 0x99, false);
      paramSlider("VCF_ENV", &synthEngine.toneL[0x9A], 0x9A, false);
      paramSlider("VCF_KEY", &synthEngine.toneL[0x9B], 0x9B, false);
      paramSlider("VCF_DYNA", &synthEngine.toneL[0x9C], 0x9C, false);
      paramSlider("VCF_MODE", &synthEngine.toneL[0x9D], 0x9D, false);
      paramSlider("VCA_LEVE", &synthEngine.toneL[0x9E], 0x9E, false);
      paramSlider("VCA_DYNA", &synthEngine.toneL[0x9F], 0x9F, false);
      paramSlider("CHORUS", &synthEngine.toneL[0xA0], 0xA0, false);
      paramSlider("LFO_WF", &synthEngine.toneL[0xA1], 0xA1, false);
      paramSlider("LFO_DELA", &synthEngine.toneL[0xA2], 0xA2, false);
      paramSlider("LFO_RATE", &synthEngine.toneL[0xA3], 0xA3, false);
      paramSlider("ENV1_ATT", &synthEngine.toneL[0xA4], 0xA4, false);
      paramSlider("ENV1_DEC", &synthEngine.toneL[0xA5], 0xA5, false);
      paramSlider("ENV1_SUS", &synthEngine.toneL[0xA6], 0xA6, false);
      paramSlider("ENV1_REL", &synthEngine.toneL[0xA7], 0xA7, false);
      paramSlider("ENV1_KEY", &synthEngine.toneL[0xA8], 0xA8, false);
      paramSlider("ENV2_ATT", &synthEngine.toneL[0xA9], 0xA9, false);
      paramSlider("ENV2_DEC", &synthEngine.toneL[0xAA], 0xAA, false);
      paramSlider("ENV2_SUS", &synthEngine.toneL[0xAB], 0xAB, false);
      paramSlider("ENV2_REL", &synthEngine.toneL[0xAC], 0xAC, false);
      paramSlider("ENV2_KEY", &synthEngine.toneL[0xAD], 0xAD, false);
      paramSlider("UNK_0xAE", &synthEngine.toneL[0xAE], 0xAE, false);
      paramSlider("VCA_MODE", &synthEngine.toneL[0xAF], 0xAF, false);
      ImGui::EndTable();

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

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void paramSlider(const char *name, int *ptr, unsigned char id, bool upper) {
  bool changed = ImGui::SliderInt(name, ptr, 0, 127);
  if (changed) {
    synthEngine.changeParam(upper, id, *ptr);
  }
}
