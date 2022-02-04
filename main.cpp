#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <chrono>
#include <thread>
#include "Chip8.h"
// Referred to this github link as inspired to get the SDL windows running
// https://github.com/JamesGriffin/CHIP-8-Emulator
uint8_t keyboard[0x10] = {SDLK_x, SDLK_1, SDLK_2, SDLK_3,
						SDLK_q, SDLK_w, SDLK_e, SDLK_a,
						SDLK_s, SDLK_d, SDLK_z, SDLK_c,
						SDLK_4, SDLK_r, SDLK_f, SDLK_v};
// Set the color of the pixels to draw
// Set to 0xffffffff or 0x00ffffff for white, 0xff for blue, 0xff00 for green, 0xff0000 for red
uint32_t color = 0xffffffff;
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: programname <Rom filepath>\n");
		return 1;
	}
	/*printf("Pick a color value in 32 bit hex for the sprites:\n0x");
	scanf("%x", &color);*/
	start:
	Chip8 chip8 = Chip8();
	int windowWidth = 1280;
	int windowHeight = 800;
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Failed to initialize everything! %s\n", SDL_GetError());
		return 1;
	}
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	window = SDL_CreateWindow(argv[1], 0, 0, windowWidth, windowHeight, 
	SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("Couldn't be initialized! %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetWindowResizable(window, SDL_TRUE);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
	
	//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
	SDL_TEXTUREACCESS_STREAMING, 64, 32);
	uint32_t pixels[64 * 32];
	SDL_Init(SDL_INIT_AUDIO);
	SDL_Event e;
	chip8.load(argv[1]);
	bool paused = false;
	bool reset = false;
	while (1) {
		if (!paused)
			chip8.cycle();
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				for (int i = 0; i < 0x10; i++) {
					if (e.key.keysym.sym == keyboard[i]) {
						chip8.keys[i] = 1;
					}
				}
				if (e.key.keysym.sym == SDLK_p)
					paused = !paused;
				if (e.key.keysym.sym == SDLK_0) {
					reset = true;
					break;
				}
				if (e.key.keysym.sym == SDLK_ESCAPE)
					break;
			}
			if (e.type == SDL_KEYUP) {
				for (int i = 0; i < 0x10; i++) {
					if (e.key.keysym.sym == keyboard[i]) {
						chip8.keys[i] = 0;
					}
				}
			}
			if (e.type == SDL_QUIT) {
				break;
			}
		}
		for (int i = 0; i < 64 * 32; i++) {
			uint8_t displaypix = chip8.display[i];
			pixels[i] = displaypix * color;
		}
		SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);	
		SDL_RenderPresent(renderer);
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
		}
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	if (reset)
		goto start;
	return 0;
}
