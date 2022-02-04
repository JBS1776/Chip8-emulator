#ifndef CHIP8
#define CHIP8

#include <stdint.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "Beep.h"
class Chip8 {
private:
	// Since I wish to eventually emulate the NES, I created methods for each
	// opcode instead of using a switch statement for easier structure
	// Idea obtained from the below link
	
	// https://austinmorlan.com/posts/chip8_emulator/#function-pointer-table
	
	void executeInstruction();
	// Do nothing
	void OP_NULL();

	// CLS
	void OP_00e0();

	// RET
	void OP_00ee();

	// JP address
	void OP_1nnn();

	// CALL address
	void OP_2nnn();

	// SE Vx, byte
	void OP_3xnn();

	// SNE Vx, byte
	void OP_4xnn();

	// SE Vx, Vy
	void OP_5xy0();

	// LD Vx, byte
	void OP_6xnn();

	// ADD Vx, byte
	void OP_7xnn();

	// LD Vx, Vy
	void OP_8xy0();

	// OR Vx, Vy
	void OP_8xy1();

	// AND Vx, Vy
	void OP_8xy2();

	// XOR Vx, Vy
	void OP_8xy3();

	// ADD Vx, Vy
	void OP_8xy4();

	// SUB Vx, Vy
	void OP_8xy5();

	// SHR Vx
	void OP_8xy6();

	// SUBN Vx, Vy
	void OP_8xy7();

	// SHL Vx
	void OP_8xye();

	// SNE Vx, Vy
	void OP_9xy0();

	// LD I, address
	void OP_annn();

	// JP V0, address
	void OP_bnnn();

	// RND Vx, byte
	void OP_cxnn();

	// DRW Vx, Vy, height
	void OP_dxyn();

	// SKP Vx
	void OP_ex9e();

	// SKNP Vx
	void OP_exa1();

	// LD Vx, DT
	void OP_fx07();

	// LD Vx, K
	void OP_fx0a();

	// LD DT, Vx
	void OP_fx15();

	// LD ST, Vx
	void OP_fx18();

	// ADD I, Vx
	void OP_fx1e();

	// LD F, Vx
	void OP_fx29();

	// LD B, Vx
	void OP_fx33();

	// LD [I], Vx
	void OP_fx55();

	// LD Vx, [I]
	void OP_fx65();
	// Construct function array below from above values
	// For some reason, we need to declare a typedef of type Function*
	// instead of just using void *
	typedef void (Chip8::*Function)();
	
	Function fun[0x10]; // For first digit in opcode
	
	Function fun0[0xf]; // if first digit == 0, get last digit
	
	Function fun8[0xf]; // if first digit == 8, let last digit
	
	Function fune[0xf]; // if first digit == 0xe, get last digit
	
	Function funf[0x66]; // if first digit == 0xf, get last two digits
	
	void Function0();
	void Function8();
	void Functione();
	void Functionf();
	uint8_t memory[0x1000];
	uint8_t registers[0x10];
	
	uint16_t pc;
	uint16_t opcode;
	uint16_t indexRegister; // 12 bits
	
	uint16_t stack[0x10]; // 16 level stack
	uint16_t sp;
	
	uint8_t delaytimer;
	uint8_t soundtimer;
	Beeper beep;
	bool soundplay;
public:
	Chip8();
	uint8_t display[64 * 32]; // 64x32 display
	uint8_t font[0xa0] = {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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
	uint8_t keys[0x10];
		/*
		keylayout:	Keyboard:
		1, 2, 3, C  1, 2, 3, 4
		4, 5, 6, D  Q, W, E, R
		7, 8, 9, E  A, S, D, F
		A, 0, B, F  Z, X, C, Y
	*/
	void load(const char *filename);
	void cycle();
};
#endif
