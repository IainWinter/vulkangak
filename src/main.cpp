#include "game/game_loop.h"
#include "TestGame.h"

int main() {
    TestGame* game = new TestGame();
    GameLoop* loop = new GameLoop(game);
    
    loop->run();

    delete game;
    delete loop;
}