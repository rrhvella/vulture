#include "./vulture-game.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

unique_ptr<VultureGame> game;

void RunFrame() 
{
    game->runFrame();
}

int main(void)
{
    game = make_unique<VultureGame>();
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(RunFrame, 0, 1);
#else
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        RunFrame();
    }
#endif

    return 0;
}
