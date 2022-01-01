#pragma once

#include "common.h"

namespace dsemu::ui {
    const int SCREEN_WIDTH = 1024;
    const int SCREEN_HEIGHT = 768;

    void init();
    void update();
    void handleEvents();
}
