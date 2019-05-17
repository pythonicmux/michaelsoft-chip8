#include <iostream>
#include "SDL2/SDL.h"
#include "chip8.hpp"
#include <chrono>
#include <thread>

using namespace std;

uint8_t keys[16] = {
	SDLK_z, SDLK_x, SDLK_c, SDLK_v,
	SDLK_a, SDLK_s, SDLK_d, SDLK_f,
	SDLK_q, SDLK_w, SDLK_e, SDLK_r,

	SDLK_1, SDLK_2, SDLK_3, SDLK_4
};

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cout << "Usage: ./michaelsoft-chip8 <ROM>" << endl;
		return 1;
	}
	
	//window dimensions, width must be 2*height
	int width = 800;
	int height = 400;

	//initialize
	Chip8 chip8 = Chip8();
	chip8.initialize();

	//load in ROM
	chip8.loadGame(argv[1]);

	//SDL window
	SDL_Window *window = NULL;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cout << "SDL initialization error" << endl;
		return 1;
	}
	window = SDL_CreateWindow("MichaelSoft Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
	width, height, SDL_WINDOW_SHOWN);

	if(window == NULL){
		cout << "Window couldn't be created" << endl;
		return 1;
	}

	//SDL renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, width, height);

	//create a texture that maps the 32x64 display of the chip 8 to the higher-res window
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	//buffer for the graphics
	uint32_t buffer[2048];
	//event placeholder for SDL
	SDL_Event event;

	//game loop
	while(1){
		chip8.emulateCycle();

		//if there's an I/O event then process it
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT) return 0;

			//key press events
			if(event.type == SDL_KEYDOWN){
				//if it's escape then end game
				if(event.key.keysym.sym == SDLK_ESCAPE) return 0;

				for(int i = 0; i < 16; i++){
					if(event.key.keysym.sym == keys[i]) chip8.key[i] = 1;
				}
			}
			//key release events
			if(event.type == SDL_KEYUP){
				for(int i = 0; i < 16; i++){
					if(event.key.keysym.sym == keys[i]) chip8.key[i] = 0;
				}
			}

			//re-draw if the draw flag is set
			if(chip8.drawFlag){
				chip8.drawFlag = false;

				for(int i = 0; i < 2048; i++){
					buffer[i] = (0xFFFFFF * chip8.gfx[i]) | 0xFF000000; // set pixel is ~0, not set pixel is 0xFF000000
				}

				//update SDL texture
				SDL_UpdateTexture(texture, NULL, buffer, 64 * sizeof(uint32_t));
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
			}

			//play sound if the sound timer > 0
		}

		//slow down emulation speed by slowing it down here 
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
}