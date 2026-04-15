#include "tetromino-pieces.h"
#include "tetromino.cpp"
#include <print>
#include <charconv>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <array>


struct _G
{
    // window width and height
    static constexpr int width {20};
    static constexpr int height {16};
    static int rotation;
    

    // window buffer and state saver
    static std::array<char, width * height> buffer;
    static std::array<char, width * height> field;
};

int _G::rotation {0};

class Piece
{
    public: // rotate the pieces
        int rotate(int x, int y, int r)
        {
            switch(r % 4)
            {
                case 0: return y * 4 + x;                       // 0 degrees
                case 1: return x * 4 + (3 - y);                 // 90 degrees
                case 2: return (3 - y) * 4 + (3 - x);           // 180 degrees
                case 3: return (3 - x) * 4 + y;                 // 270 degrees
            }
            return 0;
        }


    public: // position offsets
        int posX {8};
        int posY {1};


        // check tetromino piece movement
        bool canMoveDown {true};
        bool canMoveRight {true};
        bool canMoveLeft {true};
};


int main(int argc, char* argv[])
{
    Piece piece;

    _G::field.fill(' ');
    _G::buffer.fill(' ');

    if(argc > 2)
    {
        std::println("usage: ./out [0-4]");
        return 0;
    } else if(argc == 2)
    {
        std::from_chars_result result {std::from_chars(argv[1], argv[1] + strlen(argv[1]), _G::rotation)};

        if(result.ec != std::errc() 
                || _G::rotation < 0 
                || _G::rotation > 4){
            std::cerr << "Invalid option: " << argv[1];
            return 0;
        }
    }

    while(true)
    {
        _G::buffer.fill(' ');           // fill entire 'buffer' array with spaces

        // draw the walls: top, bottom, left, and right with the '#' character
        for(int y{0}; y < _G::height; y++)
        {
            for(int x {0}; x < _G::width; x++)
            {
                _G::buffer[y * _G::width + x] = 
                    (
                     x == 0 || x == _G::width - 1 
                     || y == 0 || y == _G::height - 1
                    ) 
                    ? '#' : _G::field[y * _G::width + x];
            }
        }


        // draw the pieces in the field and check for collisions
        for(int py {0}; py < 4; py++)
        {
            for(int px {0}; px < 4; px++)
            {
                if(getTetromino()[piece.rotate(px, py, _G::rotation)] == 'X')
                {
                    _G::buffer[(py + piece.posY) * _G::width + (px + piece.posX)] = 'X';      // draw the piece with offsetts on x and y

                    // check collision with bottom floor
                    if(_G::buffer[(py + piece.posY + 1) * _G::width + (px + piece.posX)] == '#' 
                            || _G::buffer[(py +  piece.posY + 1) * _G::width + (px + piece.posX)] == 'X')
                    { piece.canMoveDown = false; }

                    // check collision with right wall
                    if(_G::buffer[(py + piece.posY) * _G::width + (px + piece.posX + 1)] == '#' 
                            || _G::buffer[(py + piece.posY) * _G::width + (px + piece.posX + 1)] == 'X')
                    { piece.canMoveRight = false; }

                    // check collision with left wall
                    if(_G::buffer[(py + piece.posY) * _G::width + (px + piece.posX - 1)] == '#' 
                            || _G::buffer[(py + piece.posY) * _G::width + (px + piece.posX - 1)] == 'X')
                    { piece.canMoveLeft = false; }
                }
            }
        }

        // print the entire field
        for(int y {0}; y < _G::height; y++)
        {
            for(int x {0}; x < _G::width; x++)
            {
                std::print("{}", _G::buffer[y * _G::width + x]);
            }
            std::cout << '\n';
        }

        // increment  offsets if no collision detected, simulating a falling/moving effect
        if(piece.canMoveDown) piece.posY++;
        else if(!piece.canMoveDown) {     // lock the pieces in place to save the piece state after each iteration
            for(int py {}; py < 4; py++)
            {
                for(int px {}; px < 4; px++)
                {
                    if(getTetromino()[piece.rotate(px, py, _G::rotation)] == 'X')
                    {
                        _G::field[(py + piece.posY) * _G::width + (px + piece.posX)] = 'X';
                    }
                }
            }
            // reset offsets
            piece.posX = 8;
            piece.posY = 1;
        }

        if(piece.canMoveRight) piece.posX++;

        // halt the main thread for 300 milliseconds during each iteration.
        // This creates an animation effect of the piece falling down
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        std::cout << "\x1b[H\x1b[2J";     // ANSI codes to put the cursor at top left and clear screen during each iteration
    }
    return 0;
}
