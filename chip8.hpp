#pragma once
#include <stdint.h>
#include <string>

class Chip8{
    private:
        unsigned short opcode; //instruction opcode
        uint8_t memory[4096]; //memory
        uint8_t V[16]; //registers
        unsigned short I; //address register (0x0 - 0xFFF)
        unsigned short pc; //program counter (0x0 - 0xFFF)
        uint8_t delay_timer; //counts down to 0, used to time events in games
        unsigned short stack[16]; //stack is 16 "levels" (I'm guessing a level is the size of a stack frame)
        unsigned short sp;  //stack pointer
    public:
        uint8_t sound_timer; //counts down to 0, when value is nonzero it beeps
        Chip8();
        ~Chip8();
        bool drawFlag; //indicates a redraw needs to occur
        uint8_t gfx[64*32]; //screen is 32x64; pixel is either on or off in the screen 
        uint8_t key[16]; //hex keyboard from 0x0-0xF
        void initialize(); //initialize system
        void loadGame(const char *path); //load in a ROM, set memory from 0x200-0xFFF to the ROM
        void emulateCycle(); //emulate a cycle of the chip-8
};
