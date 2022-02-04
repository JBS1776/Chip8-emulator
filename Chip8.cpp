#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Beep.h"
#include "Chip8.h"

Chip8::Chip8() {
	for (int i = 0; i < 0x1000; i++) {
		if (i < 0x800) {
			if (i < 0x10) {
				keys[i] = 0;
				stack[i] = 0;
				registers[i] = 0;
			}
			display[i] = 0;
		}
		memory[i] = 0;
	}
	for (int i = 0; i < 0x50; i++) {
		memory[i + 0x50] = font[i];
	}
	pc = 0x200;
	opcode = 0;
	indexRegister = 0;
	sp = 0;
	delaytimer = 0;
	soundtimer = 0;
	soundplay = false;
	
	srand(time(NULL));
	//load instructions here
	for (int i = 0; i < 0x65; i++) {
		if (i < 0xf) {
			fun0[i] = &Chip8::OP_NULL;
			fun8[i] = &Chip8::OP_NULL;
			fune[i] = &Chip8::OP_NULL;
		}
		funf[i] = &Chip8::OP_NULL;
	}
	fun[0x0] = &Chip8::Function0;
	fun[0x1] = &Chip8::OP_1nnn;
	fun[0x2] = &Chip8::OP_2nnn;
	fun[0x3] = &Chip8::OP_3xnn;
	fun[0x4] = &Chip8::OP_4xnn;
	fun[0x5] = &Chip8::OP_5xy0;
	fun[0x6] = &Chip8::OP_6xnn;
	fun[0x7] = &Chip8::OP_7xnn;
	fun[0x8] = &Chip8::Function8;
	fun[0x9] = &Chip8::OP_9xy0;
	fun[0xa] = &Chip8::OP_annn;
	fun[0xb] = &Chip8::OP_bnnn;
	fun[0xc] = &Chip8::OP_cxnn;
	fun[0xd] = &Chip8::OP_dxyn;
	fun[0xe] = &Chip8::Functione;
	fun[0xf] = &Chip8::Functionf;
	
	fun0[0x0] = &Chip8::OP_00e0;
	fun0[0xe] = &Chip8::OP_00ee;
	
	fun8[0x0] = &Chip8::OP_8xy0;
	fun8[0x1] = &Chip8::OP_8xy1;
	fun8[0x2] = &Chip8::OP_8xy2;
	fun8[0x3] = &Chip8::OP_8xy3;
	fun8[0x4] = &Chip8::OP_8xy4;
	fun8[0x5] = &Chip8::OP_8xy5;
	fun8[0x6] = &Chip8::OP_8xy6;
	fun8[0x7] = &Chip8::OP_8xy7;
	fun8[0xe] = &Chip8::OP_8xye;
	
	fune[0xe] = &Chip8::OP_ex9e;
	fune[0x1] = &Chip8::OP_exa1;
	
	funf[0x7] = &Chip8::OP_fx07;
	funf[0xa] = &Chip8::OP_fx0a;
	funf[0x15] = &Chip8::OP_fx15;
	funf[0x18] = &Chip8::OP_fx18;
	funf[0x1e] = &Chip8::OP_fx1e;
	funf[0x29] = &Chip8::OP_fx29;
	funf[0x33] = &Chip8::OP_fx33;
	funf[0x55] = &Chip8::OP_fx55;
	funf[0x65] = &Chip8::OP_fx65;
}
void Chip8::load(const char *filename) {
	FILE * file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Unable to open file!\n");
		exit(1);
	}
	char buffer[0x1000 - 0x200];
	int c;
	int increment = 0;
	while (c != EOF) {
		c = getc(file);
		buffer[increment] = (uint8_t) c;
		increment++;
		if (increment >= 0x1000 - 0x200) {
			printf("File too large!!\n");
			exit(1);
		}
	}
	for (int i = 0; i < increment; i++) {
		memory[i + 0x200] = buffer[i];
	}
}
void Chip8::cycle() {
	opcode = (memory[pc] << 8) | memory[pc + 1];
	//printf("pc: 0x%.4x, opcode: 0x%.4x\n", pc, opcode);
	pc += 2;
	executeInstruction();
	if (delaytimer > 0)
		delaytimer--;
	if (soundtimer > 0) {
		if (soundplay) {
			//temporary solution to play sound, \a plays alert sound of OS
			//printf("\a");
			// Alternatively play the note A 440 as the beep w/ below
			beep.beep(DEFFREQ, soundtimer * 10);
			beep.wait();
			soundplay = false;
		}
		soundtimer--;
		}
}
void Chip8::executeInstruction() {
	((*this).*(fun[(opcode & 0xf000) >> 12]))();
}
void Chip8::Function0() {
	((*this).*(fun0[opcode & 0xf]))();
}
void Chip8::Function8() {
	((*this).*(fun8[opcode & 0xf]))();
}
void Chip8::Functione() {
	((*this).*(fune[opcode & 0xf]))();
}
void Chip8::Functionf() {
	((*this).*(funf[opcode & 0xff]))();
}

// Do nothing
void Chip8::OP_NULL() {
	printf("Unknown code 0x%x\n", opcode);
}

	// Clear display
void Chip8::OP_00e0() {
	for (int i = 0; i < 64 * 32; i++) {
		display[i] = 0;
	}
}
	// RET, pops function from stack
void Chip8::OP_00ee() {
	pc = stack[--sp];
}
	// JP address
void Chip8::OP_1nnn() {
	pc = opcode & 0xfff;
}

	// CALL address, push function to stack
void Chip8::OP_2nnn(){
	stack[sp++] = pc;
	pc = opcode & 0xfff;
}
	// SE Vx, byte
void Chip8::OP_3xnn(){
	if (registers[(opcode & 0xf00) >> 8] == (opcode & 0xff))
		pc += 2;
}

	// SNE Vx, byte
void Chip8::OP_4xnn(){
	if (registers[(opcode & 0xf00) >> 8] != (opcode & 0xff))
		pc += 2;
}

	// SE Vx, Vy
void Chip8::OP_5xy0(){
	if (registers[(opcode & 0xf00) >> 8] == registers[(opcode & 0xf0) >> 4])
		pc += 2;
}

	// LD Vx, byte
void Chip8::OP_6xnn(){
	registers[(opcode & 0xf00) >> 8] = opcode & 0xff;
}

	// ADD Vx, byte
void Chip8::OP_7xnn(){
	registers[(opcode & 0xf00) >> 8] += opcode & 0xff;
}

	// LD Vx, Vy
void Chip8::OP_8xy0(){
	registers[(opcode & 0xf00) >> 8] = registers[(opcode & 0xf0) >> 4];
}

	// OR Vx, Vy
void Chip8::OP_8xy1(){
	registers[(opcode & 0xf00) >> 8] |= registers[(opcode & 0xf0) >> 4];
}

	// AND Vx, Vy
void Chip8::OP_8xy2(){
	registers[(opcode & 0xf00) >> 8] &= registers[(opcode & 0xf0) >> 4];
}

	// XOR Vx, Vy
void Chip8::OP_8xy3(){
	registers[(opcode & 0xf00) >> 8] ^= registers[(opcode & 0xf0) >> 4];
}

	// ADD Vx, Vy
void Chip8::OP_8xy4(){
	registers[(opcode & 0xf00) >> 8] += registers[(opcode & 0xf0) >> 4];
	registers[0xf] = registers[(opcode & 0xf00) >> 8] > 0xff;
	registers[(opcode & 0xf00) >> 8] &= 0xff; 
}

	// SUB Vx, Vy
void Chip8::OP_8xy5(){
	registers[0xf] = registers[(opcode & 0xf00) >> 8] > registers[(opcode & 0xf0) >> 4];
	registers[(opcode & 0xf00) >> 8] -= registers[(opcode & 0xf0) >> 4];
}

	// SHR Vx
void Chip8::OP_8xy6(){
	registers[0xf] = registers[(opcode & 0xf00) >> 8] & 1;
	registers[(opcode & 0xf00) >> 8] >>= 1;
}

	// SUBN Vx, Vy
void Chip8::OP_8xy7(){
	registers[0xf] = registers[(opcode & 0xf0) >> 4] > registers[(opcode & 0xf00) >> 8];
	registers[(opcode & 0xf00) >> 8] = registers[(opcode & 0xf0) >> 4] - registers[(opcode & 0xf00) >> 8];
	
}

	// SHL Vx
void Chip8::OP_8xye(){
	// Any value > 0x7f will have 1 as most significant digit
	registers[0xf] = registers[(opcode & 0xf00) >> 8] > 0x7f;
	registers[(opcode & 0xf00) >> 8] <<= 1;
}

	// SNE Vx, Vy
void Chip8::OP_9xy0(){
	if (registers[(opcode & 0xf00) >> 8] != registers[(opcode & 0xf0) >> 4]) {
		pc += 2;
	}
}

	// LD I, address
void Chip8::OP_annn(){
	indexRegister = opcode & 0xfff;
}

	// JP V0, address
void Chip8::OP_bnnn(){
	pc = (opcode & 0xfff) + registers[0];
}

	// RND Vx, byte
void Chip8::OP_cxnn(){
	registers[(opcode & 0xf00) >> 8] = (rand() & 0xff) & (opcode & 0xff);
}

	// DRW Vx, Vy, height
void Chip8::OP_dxyn(){
	//printf("DRAW!\n");
	// % the cords by 64 and 32 respectively to wrap around edges
	// We could also use & 63 and & 31 so we don't have to use mode, slight speedup
	uint8_t x = registers[(opcode & 0xf00) >> 8] & 63;
	uint8_t y = registers[(opcode & 0xf0) >> 4] & 31;
	uint8_t n = opcode & 0xf;
	registers[0xf] = 0;
	for (int i = 0; i < n; i++) {
		// get pixel from memory
		uint8_t pixel = memory[indexRegister + i];
		// pixel is 8 bits wide
		for (int j = 0; j < 8; j++) {
			// display is 1-dimensional but (Vx, Vy) is a 2D vector
			// Consider index of display such as the below:
			// x=0..63  x=0..63
			// {0..63}, {64..127}, etc...
			//  y = 0    y = 1
			
			// sprite var gets the most significant bit of the pixel as the jth pixel
			uint8_t sprite = pixel & (0x80 >> j);
			if (sprite) {
				// (y + i) << 6 is the same as (y + i) * 64
				if (display[((y + i) << 6) + (x + j)])
					registers[0xf] = 1;
				display[((y + i) << 6) + (x + j)] ^= 1;
			}
		}
	}
}

	// SKP Vx
void Chip8::OP_ex9e(){
	if (keys[registers[(opcode & 0xf00) >> 8]])
		pc += 2;
}

	// SKNP Vx
void Chip8::OP_exa1(){
	if (!keys[registers[(opcode & 0xf00) >> 8]])
		pc += 2;
}

	// LD Vx, DT
void Chip8::OP_fx07(){
	registers[(opcode & 0xf00) >> 8] = delaytimer;
}

	// LD Vx, K
void Chip8::OP_fx0a(){
	uint8_t i = 0;
	for (i = 0; i < 0x10; i++) {
		if (keys[i]) {
			registers[(opcode & 0xf00) >> 8] = i;
			break;
		}
	}
	if (i == 0x10)
		pc -= 2;
}

	// LD DT, Vx
void Chip8::OP_fx15(){
	delaytimer = registers[(opcode & 0xf00) >> 8];
}

	// LD ST, Vx
void Chip8::OP_fx18(){
	soundtimer = registers[(opcode & 0xf00) >> 8];
	soundplay = true;
}

	// ADD I, Vx
void Chip8::OP_fx1e(){
	// set registers[0xf] to 1 if indexRegister + v[x] > 12 bits
	registers[0xf] = (indexRegister + registers[(opcode & 0xf00) >> 8]) > 0xfff;
	indexRegister += registers[(opcode & 0xf00) >> 8];
}

	// LD F, Vx
void Chip8::OP_fx29(){
	// index location in memory, sprites are 0x0 - 0xf and 5 bytes wide
	// 0xff * 5 < 0xffff so no need to & 0xfff the result
	indexRegister = 0x50 + (registers[(opcode & 0xf00) >> 8] * 5);
}

	// LD B, Vx
void Chip8::OP_fx33(){
	memory[indexRegister] = registers[(opcode & 0xf00) >> 8] / 100;
	memory[indexRegister + 1] = (registers[(opcode & 0xf00) >> 8] / 10) % 10;
	memory[indexRegister + 2] = registers[(opcode & 0xf00) >> 8] % 10;
}

	// LD [I], Vx
void Chip8::OP_fx55(){
	for (int i = 0; i <= (opcode & 0xf00) >> 8; i++) {
		memory[i + indexRegister] = registers[i];
	}
}

	// LD Vx, [I]
void Chip8::OP_fx65(){
	for (int i = 0; i <= (opcode & 0xf00) >> 8; i++) {
		registers[i] = memory[i + indexRegister];
	}
}
