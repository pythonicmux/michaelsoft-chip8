#include "chip8.hpp"
#include <iostream>
#include <stdlib.h>

using namespace std;

//taken from writeup at http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8(){}

Chip8::~Chip8(){}

//clears the screen
void clear_screen(Chip8 *c){
    for(int i = 0; i < 64*32; i++){
        c->gfx[i] = 0;
    }
}

//initialize system
void Chip8::initialize(){
    //pc starts at ROM, clear opcode, addr pointer (I) and sp
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    //clear display 
    clear_screen(this);
    //clear stack, keys, registers
    for(int i = 0; i < 16; i++){
        stack[i] = 0;
        V[i] = 0;
        key[i] = 0;
    } 
    //clear memory
    for(int i = 0; i < 4096; i++){
        memory[i] = 0;
    }
    //load fontset
    for(int i = 0; i < 80; i++){
        memory[i] = chip8_fontset[i];
    }
    //reset timers
    delay_timer = 0;
    sound_timer = 0;
}

//load in a ROM, set memory from 0x200-0xFFF to the ROM
void Chip8::loadGame(const char *path){
    int bufferSize = 0;
    int result;
    char *buffer;
    FILE *rom = fopen(path, "rb");
    if(rom == NULL){
        cout << "Invalid filename for ROM" << endl;
        exit(0);
    } 
    
    //get size of the rom
    fseek(rom, 0L, SEEK_END); //go to end and then ask for position
    bufferSize = ftell(rom);
    rewind(rom); //rom is at start again
    
    //get the bytes from the rom
    buffer = (char *) malloc(sizeof(char) * bufferSize);
    if(buffer == NULL){
        cout << "Out of memory, can't malloc buffer for ROM" << endl;
        exit(1);
    }
    result = fread(buffer, 1, bufferSize, rom);
    if(result != bufferSize){
        cout << "ROM read in incorrectly" << endl;
        exit(1);
    }

    //read in buffer into chip8 memory if it's a valid size
    if(bufferSize <= 4096 - 0x200){
        for(int i = 0; i < bufferSize; i++){
            memory[i+0x200] = buffer[i];
        }
    }
    else{
        cout << "Invalid rom; memory too big" << endl;
        exit(0);
    }
} 

//emulate a cycle of the chip-8
void Chip8::emulateCycle(){
    //for drawing the sprite later
    int x,y,height,width;
    bool changed, pixelSet;

    //fetch
    opcode = memory[pc] << 8 | memory[pc+1];
    //decode with switch, execute in the conditional
    //note that the and is because the first 4 bits are the code, the rest is an address
    switch(opcode & 0xF000){
        //if it's all zeroes, then it must be 0x00E0/EE:
        case 0x0000:
            switch(opcode & 0x00FF){
                //clear the screen
                case 0x00E0:
                    clear_screen(this);
                    drawFlag = true;
                    pc += 2;
                    break;
                //return from subroutine
                case 0x00EE:
                    //next one up on stack is the return addr, so set pc to that and then go to next instruction
                    sp -= 1;
                    pc = stack[sp];
                    pc += 2;
                    break;
            }
            break;
        //1NNN: goto 0xNNN
        case 0x1000:
            pc = opcode & 0xFFF;
            break;
        //2NNN: call *(0xNNN)() (it's a function, not an instruction)
        case 0x2000:
            stack[sp] = pc; 
            sp += 1;
            pc = opcode & 0xFFF;
            break;
        //3XNN: if Vx == NN then skip next instruction
        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0xFF)) pc += 2;
            pc += 2;
            break;
        //4XNN: if Vx != NN then skip next instruction
        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0xFF)) pc += 2;
            pc += 2;
            break;
        //5XY0: if Vx == Vy skip next instruction
        case 0x5000:
            if(V[(opcode & 0xF00) >> 8] == V[(opcode & 0xF0) >> 4]) pc += 2;
            pc += 2;
            break;
        //6XNN: Vx == NN
        case 0x6000:
            V[(opcode & 0xF00) >> 8] = (uint8_t) (opcode & 0xFF);
            pc += 2;
            break;
        //7XNN: Vx += NN
        case 0x7000:
            V[(opcode & 0xF00) >> 8] += (uint8_t) (opcode & 0xFF);
            pc += 2;
            break;
        //8XY0: register bitwise operations
        case 0x8000:
            switch(opcode & 0xF){
                case 0:
                    V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF0) >> 4];
                    break;
                case 1:
                    V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] | V[(opcode & 0xF0) >> 4];
                    break;
                case 2:
                    V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] & V[(opcode & 0xF0) >> 4];
                    break;
                case 3:
                    V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] ^ V[(opcode & 0xF0) >> 4];
                    break;
                case 4:
                    //overflow check
                    V[0xF] = 0;
                    if(V[(opcode & 0xF00) >> 8] > (0xFF - V[(opcode & 0xF0) >> 4])){
                        V[0xF] = 1;
                    }
                    //add
                    V[(opcode & 0xF00) >> 8] += V[(opcode & 0xF0) >> 4];
                    break;
                case 5:
                    V[(opcode & 0xF00) >> 8] -= V[(opcode & 0xF0) >> 4];
                    break;
                case 6:
                    V[0xF] = V[(opcode & 0xF00) >> 8] & 1;
                    V[(opcode & 0xF00) >> 8] >>= 1;
                    break;
                case 7:
                    V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF0) >> 4] - V[(opcode & 0xF00) >> 8];
                    break;
                case 0xE:
                    V[0xF] = (V[(opcode & 0xF00) >> 8] & 8) >> 3;
                    V[(opcode & 0xF00) >> 8] <<= 1;
                    break; 
            }
            pc += 2;
            break;
        //9XY0: if Vx != Vy skip next instruction
        case 0x9000:
            if(V[(opcode & 0xF00) >> 8] != V[(opcode & 0xF0) >> 4]) pc += 2;
            pc += 2;
            break;
        //ANNN: set address reg
        case 0xA000:
            I = opcode & 0xFFF;
            pc += 2;
            break;
        //BNNN: pc = v0 + NNN
        case 0xB000:
            pc = V[0] + (opcode & 0xFFF);
            break;
        //CXNN: Vx = rand() & NN
        case 0xC000:
            V[(opcode & 0xF00) >> 8] = (rand() % 256) & (opcode & 0xFF);
            pc += 2;
            break;
        //DXYN: Draw sprite at (Vx, Vy), width 8 pixels, height of N- each row of 8 is read as bits from *(I)
        //Vf is set to 1 if any pixels flip from set to unset during drawing, and 0 if it doesn't happen
        case 0xD000:
            height = (opcode & 0xF);
            x = V[(opcode & 0xF00) >> 8];
            y = V[(opcode & 0xF0) >> 4];
            width = 8;
            changed = false;
            V[0xF] = 0;
            
            //the next (height) lines in I represent each row of the sprite
            for(int r = 0; r < height; r++){
                for(int c = 0; c < width; c++){
                    //see if the pixel is drawn currently
                    pixelSet = (gfx[64*(y+r) + (x+c)] == 1);
                    //xor the pixel
                    gfx[64*(y+r) + (c+x)] ^= (memory[I+r] & (0x80 >> c)) >> (width-c-1);
                    //if the pixel was set and now it isn't, then set the changed flag
                    if(changed == false){
                        changed = pixelSet && (gfx[64*(y+r) + (c+x)] == 0);
                    }
                }
            }
            //if there was a set to unset change then set Vf
            if(changed){
                //apparently Vf is set here for collision detection
                V[0xF] = 1;
            }
            drawFlag = 1;
            pc += 2;
            break;
        //EX9E:if key stored in Vx is pressed, skip instruction. 
        //EXA1: if it isn't pressed, skip instruction
        case 0xE000:
            switch(opcode & 0xFF){
                case 0x9E:
                    if(key[V[(opcode & 0xF00) >> 8]]) pc += 2;
                    pc += 2;
                    break;
                case 0xA1:
                    if(!key[V[(opcode & 0xF00) >> 8]]) pc += 2;
                    pc += 2;
                    break;
            }
            break;
        //F000: various
        case 0xF000:
            switch(opcode & 0xFF){
                case 0x7:
                    V[(opcode & 0xF00) >> 8] = delay_timer;
                    break;
            }
            pc += 2;
            break;
        default:
            cout << "Unknown opcode called: 0x" << std::hex << opcode << std::dec << endl;
    }
    //update timers
    if(delay_timer > 0) delay_timer--;
    if(sound_timer > 0){
        cout << "BEEP" << endl;
        sound_timer--;
    }
}
