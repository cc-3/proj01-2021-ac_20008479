#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitSigner(unsigned int field, unsigned int size) {
unsigned int n_field_bits = field & ((1U << size) - 1);

	// 1 -> negative; 0 -> positive
	unsigned int msb = field & (1U << (size - 1));

	unsigned int mask = msb ? ((1UL << 32) - 1) : 0;

	mask <<= size;

	mask ^= n_field_bits;

	return mask;
}


/* Remember that the offsets should return the offset in BYTES */

int get_branch_offset(Instruction instruction) {
  int offset = 0x00000000;

	offset |= instruction.sbtype.imm5 & 0x0000001e; 

	offset |= (instruction.sbtype.imm7 << 5) & 0x000007e0;

	offset |= (instruction.sbtype.imm5 << 11) & 0x00000800; 

	offset |= (instruction.sbtype.imm7 << 6) & 0x00001000; 

	return sign_extend_number(offset, 13);
}


int get_jump_offset(Instruction instruction) {
 int offset = 0x00000000;

	offset |= (instruction.ujtype.imm >> 8) & 0x000007fe; 

	offset |= (instruction.ujtype.imm << 3) & 0x00000800; 

	offset |= (instruction.ujtype.imm << 12) & 0x000ff000; 

	offset |= (instruction.ujtype.imm << 2) & 0x00100000;

	return sign_extend_number(offset, 21);
}


int get_store_offset(Instruction instruction) {
  int offset = 0x00000000;

	offset |= instruction.stype.imm5 & 0x0000001f; 

	offset |= (instruction.stype.imm7 << 5) & 0x00000fe0; 

	return sign_extend_number(offset, 12);

}


void handle_invalid_instruction(Instruction instruction) {
  printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}


void handle_invalid_read(Address address) {
  printf("Bad Read. Address: 0x%08x\n", address);
  exit(-1);
}


void handle_invalid_write(Address address) {
  printf("Bad Write. Address: 0x%08x\n", address);
  exit(-1);
}
