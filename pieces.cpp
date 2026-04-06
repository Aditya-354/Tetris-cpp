#include "tetromino-pieces.h"

namespace Pieces
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

    std::string getTetromino()
    {
        return tetromino_2;
    }
}
