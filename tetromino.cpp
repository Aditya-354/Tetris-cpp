#include "tetromino-pieces.h"

namespace Tetromino
{
    std::string tetromino_1 {
        "..X."
        "..X."
        "..X."
        "..X."
    };

    std::string tetromino_2 {
        "...."
        ".XX."
        ".XX."
        "...."
    };

}

std::string getTetromino()
{
    return Tetromino::tetromino_2;
}
