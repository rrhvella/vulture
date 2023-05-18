#pragma once

#include "memory"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "flecs.h"

#include "./components.hpp"

using namespace std;
using namespace flecs;

class VultureGame
{
public:
    unique_ptr<int> highScore;
    unique_ptr<world> ecs;
    
    void initialize();
    void runFrame();

    VultureGame();
    ~VultureGame();
};