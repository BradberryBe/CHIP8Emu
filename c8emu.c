#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Based on the CHIP-8 emulator found on emulator101.com
 * @Author: Ben Bradberry
 * @Date: 12/20/19
 */
FILE *file;
char* filename = NULL;

void disassemble(uint8_t *buffer, int pc)
{
	uint8_t *code = &buffer[pc];
	uint8_t firstnib = (code[0] >> 4);
	uint8_t lastnib = (code[1] >> 4);
	uint8_t addresshi;
	uint8_t reg;

	printf("%04x %02x %02x ", pc, code[0], code[1]);
	switch (firstnib)
	{
		// 0x0NNN
		// 0x00E0	CLS
		// 0x00EE	RTS
		case 0x00:
			switch (code[1])
			{
				case 0xe0:
					printf("%-10s", "CLS");
					break;
				case 0xee:
					printf("%-10s", "RTS");
					break;
				default:
					printf("UNKNOWN 0");
					break;
			}
			break;
	
		// 0x1NNN	JUMP
		// Flow
		case 0x01:
			// Jumps to address NNN
			printf("%-10s %01X%02X", "JUMP", code[0]&0xf, code[1]);
			break;

		// 0x2NNN	CALL 
		// Flow
		case 0x02:
			// Calls subroutine at NNN
			printf("%-10s %01X%02X", "CALL", code[0]&0xf, code[1]);
			break;

		// 0x3XNN	SKIP.EQ
		// Conditional
		case 0x03:
			// psuedo: if(VX==NN)
			// Skips the next instruction if VX equals NN
			printf("%-10s V%01X, V%02X", "SKIP.EQ", code[0]&0xf, code[1]);
			break;

		// 0x4XNN	SKIP.NE
		// Conditional
		case 0x04:
			// pseudo: if(VX!=NN)
			// skips the next instruction if VX doesn't equal NN
			printf("%-10s V%01X, V%02X", "SKIP.NE", code[0]&0xf, code[1]);
			break;

		// 0x5XY0	SKIP.EQ
		// Conditional
		case 0x05:
			// psuedo: if(Vx==Vy)
			// skips the next instruction if VX equals VY
			printf("%-10s V%01X, V%01X", "SKIP.EQ", code[0]&0xf, code[1]>>4);
			break;

		// 0x6XNN	MVI
		// Constant
		case 0x06:
			// pseudo: Vx = NN
			// sets Vx to NN
			reg = code[0] & 0x0f;
			printf("%-10s V%01X,#$%02x", "MVI", reg, code[1]);
			break;

		// 0x7XNN
		// Constant
		case 0x07:
			// pseudo: Vx += NN
			// Adds NN to Vx (carry flag is not changed)
			printf("%-10s V%01X,#$%02x", "ADI", code[0]&0xf, code[1]);
			break;

		// 0x8XY0	MOV
		// Assign

		// 0x8XY1	OR
		// 0x8XY2	ANDXOR
		// 0x8XY3	XOR
		// BitOp
		
		// 0x8XY4	ADD
		// 0x8XY5	SUB
		// Math
		
		// 0x8XY6	SHR
		// BitOp
		
		// 0x8XY7	SUBB (backwards subtract, VX=VY-VX)
		// Math
		
		// 0x8XYE	SHL
		// BitOp
		case 0x08:
			switch(code[1]&0x0f){
				case 0x0:
					printf("%-10s V%01X,V%01X", "MOV.", code[0]&0xf, code[1]>>4);
					break;					
				case 0x1:
					printf("%-10s V%01X,V%01X", "OR.", code[0]&0xf, code[1]>>4);
					break;
				case 0x2:
					printf("%-10s V%01X,V%01X", "AND.", code[0]&0xf, code[1]>>4);
					break;
				case 0x3:
					printf("%-10s V%01X,V%01X", "XOR.", code[0]&0xf, code[1]>>4);
					break;
				case 0x4:
					printf("%-10s V%01X,V%01X", "ADD.", code[0]&0xf, code[1]>>4);
					break;
				case 0x5:
					printf("%-10s V%01X,V%01X", "SUB.", code[0]&0xf, code[1]>>4);
					break;
				case 0x6:
					printf("%-10s V%01X,V%01X", "SHR.", code[0]&0xf, code[1]>>4);
					break;
				case 0x7:
					printf("%-10s V%01X,V%01X", "SUBB.", code[0]&0xf, code[1]>>4);
					break;
				case 0xe:
					printf("%-10s V%01X,V%01X", "SHL.", code[0]&0xf, code[1]>>4);
					break;
				default:
					printf("UNKOWN 8");
					break;
			}
			break;

		// 0x9XY0	SKIP.NE
		// Conditional
		case 0x09:
			printf("%-10s V%01X,V%01X", "SKIP.NE", code[0]&0xf, code[1]>>4);
			break;
		
		// 0xANNN
		// Memory
		case 0x0a:
			addresshi = code[0] & 0x0f;
			printf("%-10s I,#$%01x%02x", "MVI", addresshi, code[1]);
			break;

		// 0xBNNN
		// Flow
		case 0x0b:
			printf("b not handled yet");
			break;

		// 0xCXNN
		// Rand
		case 0x0c:
			printf("c not handled yet");
			break;

		// 0xDXYN	SPRITE
		// Disp
		case 0x0d:
			printf("d not handled yet");
			break;

		// 0xEX9E	SKIP.Key
		// 0xEXA1 SKIP.NOKEY
		// Keyop
		case 0x0e:
			printf("e not handled yet");
			break;

		// OxFX07	MOV
		// 0xFX0A	WAITKEY
		// 0xFX15	MOV
		// 0xFX18	MOV
		// 0xFX1E	ADD
		// 0xFX29	SPRITECHAR
		// 0xFX33	MOVBCD
		// 0xFX55	MOVM
		// 0xFX65	MOVM
		case 0x0f:
			printf("f not handled yet");
			break;
	}
}


int main(int argc, char**argv)
{
	file = fopen(argv[1], "rb");
	if (file == NULL)
	{
		printf("error: Couldn't open %s\n", argv[1]);
		exit(1);
	}

	// Get the file size
	fseek(file, 0L, SEEK_END);
	int fsize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	// CHIP-8 convention puts programs in memory at 0x200
	// They will all have hardcoded addresses expecting that
	//
	// Read the file into memory at 0x200 and close it.
	unsigned char *buffer=malloc(fsize+0x200);
	fread(buffer+0x200, fsize, 1, file);
	fclose(file);

	int pc=0x200;
	while (pc < (fsize+0x200))
	{
		disassemble(buffer, pc);
		pc += 2;
		printf("\n");
	}

	return 0;
}
  
