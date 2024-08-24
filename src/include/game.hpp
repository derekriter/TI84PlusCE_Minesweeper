#pragma once

namespace Game {
    enum Difficulty {
        BEGINNER = 0,
        INTERMEDIATE = 1,
        EXPERT = 2,
        INSANE = 3
    };

    extern Difficulty currentDifficulty;
}
