#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <imgui.h>
#include <imgui_internal.h>
#include <imfilebrowser.h>
#include <GL/gl3w.h>
#include <SDL2/SDL.h>

//#include "audio.h"
//#include "cpu.h"
//#include "graphics.h"
//#include "gui.h"
//#include "input.h"
//#include "memory.h"
//#include "games/games.h"
typedef struct keymap_s {
  int start;
  int startDown;

  int select;
  int selectDown;

  int left;
  int leftDown;

  int up;
  int upDown;

  int right;
  int rightDown;

  int down;
  int downDown;

  int b;
  int bDown;

  int a;
  int aDown;
} keymap_t;


typedef struct settings_s {
  int debug;
  keymap_t keymap;
} settings_t;
class Application {
//  // GB subcomponents
//  CPU cpu;
//  Graphics graphics;
//  Input input;
//  Memory memory;
//  Audio audio;
//
//  // Emulator subcomponents
//  GUI gui;
//  settings_t settings;
//
//  // State
//  std::string filename;
 public:
  void run();


};
