#include "pieces.cpp"
#include <print>
#include <charconv>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <array>

constexpr int width {20};
constexpr int height {16};

std::array<char, width * height> buffer;
std::array<char, width * height> field;

// offsets
int posX {8};
int posY {1};

// rotate the pieces
constexpr int rotate(int x, int y, int r)
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

int main(int argc, char* argv[])
{
    field.fill(' ');
    buffer.fill(' ');
    int r {0};
    if(argc > 2)
    {
        std::println("usage: ./out [0-4]");
        exit(0);
    } else if(argc == 2)
    {
        std::from_chars_result result {std::from_chars(argv[1], argv[1] + strlen(argv[1]), r)};

        if(result.ec != std::errc() || r < 0 || r > 4){
            std::cerr << "Invalid flag: " << argv[1];
            return 0;
        }
    }

    while(true)
    {
        buffer.fill(' ');           // fill entire 'buffer' array with spaces

        // draw the walls: top, bottom, left, and right with the '#' character
        for(int y{0}; y < height; y++)
        {
            for(int x {0}; x < width; x++)
            {
                buffer[y*width+x] = (x == 0 || x == width-1 || y == 0 || y == height-1) ? '#' : field[y * width + x];
            }
        }

        [[maybe_unused]]bool canMoveDown {true};
        [[maybe_unused]]bool canMoveRight {true};
        [[maybe_unused]]bool canMoveLeft {true};

        // draw the pieces in the field and check for collisions
        for(int py {0}; py < 4; py++)
        {
            for(int px {0}; px < 4; px++)
            {
                if(Pieces::getTetromino()[rotate(px, py, r)] == 'X')
                {
                    buffer[(py + posY) * width + (px + posX)] = 'X';      // draw the piece with offsetts on x and y

                    // check collision with bottom floor
                    if(buffer[(py + posY + 1) * width + (px + posX)] == '#' || buffer[(py +  posY + 1) * width + (px + posX)] == 'X') 
                    { canMoveDown = false; }

                    // check collision with right wall
                    if(buffer[(py + posY) * width + (px + posX + 1)] == '#' || buffer[(py + posY) * width + (px + posX + 1)] == 'X') 
                    { canMoveRight = false; }

                    // check collision with left wall
                    if(buffer[(py + posY) * width + (px + posX - 1)] == '#' || buffer[(py + posY) * width + (px + posX - 1)] == 'X') 
                    { canMoveLeft = false; }
                }
            }
        }

        // print the entire field
        for(int y {0}; y < height; y++)
        {
            for(int x {0}; x < width; x++)
            {
                std::print("{}", buffer[y * width + x]);
            }
            std::println("");
        }

        // increment  offsets if no collision detected, simulating a falling/moving effect
        if(canMoveDown) posY++;
        else if(!canMoveDown) {     // lock the pieces in place to save the game state after each iteration
            for(int py {}; py < 4; py++)
            {
                for(int px {}; px < 4; px++)
                {
                    if(Pieces::getTetromino()[rotate(px, py, r)] == 'X')
                    {
                        field[(py + posY) * width + (px + posX)] = 'X';
                    }
                }
            }
            // reset offsets
            posX = 8;
            posY = 1;
        }

        if(canMoveRight) posX++;

        // halt the main thread for 300 milliseconds during each iteration. This creates an animation effect of the piece falling down
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        std::cout << "\x1b[H\x1b[2J";     // ANSI codes to put the cursor at top left and clear screen during each iteration
    }
    return 0;
}
