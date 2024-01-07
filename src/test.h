#pragma once

#include "game/entity.h"

struct Player {
    
};

struct TestScene : v2EntitySceneData {

    Player player;

    v2EntitySingleton<Player> view_player;

    TestScene() {

    }
};