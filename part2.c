#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"


// forward declarations
void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jalr(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_auipc(Instruction, Processor *);
void execute_lui(Instruction, Processor *);


void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {
  /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
   switch(instruction.opcode) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x33:
            execute_rtype(instruction, processor);
            break;

        case 0x03:
            execute_load(instruction, processor, memory);
            break;

        case 0x13:
            execute_itype_except_load(instruction, processor);
            break;

        case 0x73:
            execute_ecall(processor, memory);
            break;

        case 0x23:
            execute_store(instruction, processor, memory);
            break;

        case 0x63:
            execute_branch(instruction, processor);
            break;

        case 0x37:
            execute_lui(instruction, processor);
            break;

        case 0x6f:
            execute_jal(instruction, processor);
            break;

        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}


void execute_rtype(Instruction instruction, Processor *processor) {
   switch(instruction.rtype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            if (instruction.rtype.funct7 == 0x00) {
                // add
                processor->R[instruction.rtype.rd] = (Word) (((sWord)
                    processor->R[instruction.rtype.rs1]) + ((sWord)
                    processor->R[instruction.rtype.rs2]));
            } else if (instruction.rtype.funct7 == 0x01) {
                // mul
                processor->R[instruction.rtype.rd] = (Word) (((sWord)
                    processor->R[instruction.rtype.rs1]) * ((sWord)
                    processor->R[instruction.rtype.rs2]));
            } else {
                // sub
                processor->R[instruction.rtype.rd] = (Word) (((sWord)
                    processor->R[instruction.rtype.rs1]) - ((sWord)
                    processor->R[instruction.rtype.rs2]));
            }
            break;

        case 0x1:
            if (instruction.rtype.funct7 == 0x00) {
                // sll
                processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] <<
                processor->R[instruction.rtype.rs2];
            } else {
                // mulh
                processor->R[instruction.rtype.rd] = (Word) ((sDouble)
                    processor->R[instruction.rtype.rs1]) + ((sDouble)
                    processor->R[instruction.rtype.rs2]);
            }
            break;

        case 0x2:
            // slt
            processor->R[instruction.rtype.rd] = (((sWord) processor->R[instruction.rtype.rs1])
                < ((sWord) processor->R[instruction.rtype.rs2]))? 1 : 0;
            break;

        case 0x4:
            if (instruction.rtype.funct7 == 0x00) {
                // xor
                processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] ^
                processor->R[instruction.rtype.rs2];
            } else if(instruction.rtype.funct7 == 0x01){

                int rs1 = processor->R[instruction.rtype.rs1];
                int  rs2 = processor->R[instruction.rtype.rs2];

                if (rs2 == 0) {
                    processor->R[instruction.rtype.rd] = -1;                // division entre cero
                    processor->PC += 4;
                    break;

                } 

                else if (rs1 == 0x80000000 && rs2 == 0xffffffff ) {
                    processor->R[instruction.rtype.rd] = rs1;               // overflow
                    processor->PC += 4;
                    break;
                } 


                else {
                    processor->R[instruction.rtype.rd] = rs1 / rs2;
                    processor->PC += 4;
                    break;

                }
      }



                
            
            break;

        case 0x5:
            if (instruction.rtype.funct7 == 0x00) {
                // srl
                processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] >>
                processor->R[instruction.rtype.rs2];
            } else {
                // sra
                processor->R[instruction.rtype.rd] = (Word) (((sWord)
                    processor->R[instruction.rtype.rs1]) >> ((Word)
                    processor->R[instruction.rtype.rs2]));
            }
            break;

        case 0x6:
            if (instruction.rtype.funct7 == 0x00) {
                // or
                processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] |
                processor->R[instruction.rtype.rs2];
            } 
                // rem
            else if(instruction.rtype.funct7 == 0x01){
                int rs1 = processor->R[instruction.rtype.rs1];
                int  rs2 = processor->R[instruction.rtype.rs2];

                if (rs2 == 0) {
                    processor->R[instruction.rtype.rd] = rs1;   

                }

                else if (rs1 == -2147483648 && rs2 == -1) {
                    processor->R[instruction.rtype.rd] = 0;

                }

                else{
                    processor->R[instruction.rtype.rd] = rs1%rs2;
                }

                processor->PC+=4;
                break;
            }
            break;

        case 0x7:
            // and
            processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] &
                processor->R[instruction.rtype.rs2];
            break;

        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_itype_except_load(Instruction instruction, Processor *processor) {
   int shiftOp;
    shiftOp = -1;
    switch(instruction.itype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            // addi
            processor->R[instruction.itype.rd] = (Word) (((sWord)
                processor->R[instruction.itype.rs1]) + bitSigner(instruction.itype.imm, 12));
            break;

        case 0x1:
            // slli
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] <<
                bitSigner(instruction.itype.imm, 12);
            break;

        case 0x2:
            // slti
            processor->R[instruction.itype.rd] = (((sWord) processor->R[instruction.itype.rs1])
                < bitSigner(instruction.itype.imm, 12))? 1 : 0;
            break;

        case 0x4:
            // xori
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] ^
                bitSigner(instruction.itype.imm, 12);
            break;

        case 0x5:
            shiftOp = (instruction.itype.imm & 0x01f);
            if ((instruction.itype.imm & 0xfe0) == 0x00) {
                // srli
                processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] >> shiftOp;
            } else {
                // srai
                processor->R[instruction.itype.rd] = (Word) (((sWord)
                    processor->R[instruction.itype.rs1]) >> shiftOp);
            }
            break;

        case 0x6:
            // ori
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] |
                bitSigner(instruction.itype.imm, 12);
            break;

        case 0x7:
            // andi
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] &
                bitSigner(instruction.itype.imm, 12);
            break;

        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}


void execute_ecall(Processor *p, Byte *memory) {
   switch(p->R[10]) { // What do we switch on?
        /* YOUR CODE HERE */
        case 1:
            printf("%d", p->R[11]);
            p->PC += 4;
            break;

        case 10:
            printf("exiting the simulator\n");
            exit(0);
            break;

        default: // undefined ecall
            printf("Illegal ecall number %d\n", p->R[10]); // What stores the ecall arg?
            exit(-1);
            break;
    }
}


void execute_branch(Instruction instruction, Processor *processor) {
  int banchcharrs;
  banchcharrs = 4;
    /* Remember that the immediate portion of branches
       is counting half-words, so make sure to account for that. */
    switch(instruction.sbtype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
        if(processor->R[instruction.sbtype.rs1] == processor->R[instruction.sbtype.rs2]){
            banchcharrs = (sHalf) get_branch_offset(instruction);
        }
        break;

        case 0x1:
        if(processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]){
            banchcharrs = (sHalf) get_branch_offset(instruction);
        }
        break;

        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
      
    }
   processor->PC += banchcharrs;
}


void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
  switch(instruction.itype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            // lb
            processor->R[instruction.itype.rd] = bitSigner(load(memory, ((sWord)processor->R[instruction.itype.rs1]) + ((sWord) bitSigner(instruction.itype.imm, 12)), LENGTH_BYTE, 0),  8);
            break;

        case 0x1:
            // lh
            processor->R[instruction.itype.rd] = bitSigner(load(memory, ((sWord)
                processor->R[instruction.itype.rs1]) + ((sWord)
                bitSigner(instruction.itype.imm, 12)), LENGTH_HALF_WORD, 1), 16);
            break;

        case 0x2:
            // lw
            processor->R[instruction.itype.rd] = load(memory, ((sWord)
                processor->R[instruction.itype.rs1]) + ((sWord)
                bitSigner(instruction.itype.imm, 12)), LENGTH_WORD,0);
            break;

        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}


void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
   switch(instruction.stype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            // sb
            store(memory, ((sWord) processor->R[instruction.stype.rs1]) + ((sWord) get_store_offset(instruction)), LENGTH_BYTE, processor->R[instruction.stype.rs2], 1 );
            break;

        case 0x1:
            // sh
            store(memory, ((sWord) processor->R[instruction.stype.rs1]) + ((sWord) get_store_offset(instruction)), LENGTH_HALF_WORD, processor->R[instruction.stype.rs2], 1);
            break;

        case 0x2:
            // sw
            store(memory, ((sWord) processor->R[instruction.stype.rs1]) + ((sWord) get_store_offset(instruction)), LENGTH_WORD, processor->R[instruction.stype.rs2], 1);
            break;

        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}


void execute_jalr(Instruction instruction, Processor *processor) {
    int nextPC =(sWord) processor->PC + bitSigner(get_jump_offset(instruction), 21);
  /* YOUR CODE HERE */
  processor->R[instruction.itype.rd] = processor->PC + 4;  
   processor->PC = nextPC;
   


}


void execute_jal(Instruction instruction, Processor *processor) {
     //unsigned int rd = instruction.ujtype.rd;
  processor->R[instruction.ujtype.rd] = processor->PC + 4;
  processor->PC += get_jump_offset(instruction);
}


void execute_auipc(Instruction instruction, Processor *processor) {
  processor->R[instruction.utype.rd] = processor->PC + ((Word)instruction.utype.imm << 12);
  processor->PC += 4;

}


void execute_lui(Instruction instruction, Processor *processor) {
  int imm = (instruction.utype.imm << 12);
    /* YOUR CODE HERE */
    processor->R[instruction.utype.rd] = imm;
    processor->PC += 4;
}


/* Checks that the address is aligned correctly */
int check(Address address, Alignment alignment) {
  if (address > 0 && address < MEMORY_SPACE) {
    // byte align
    if (alignment == LENGTH_BYTE) return 1;
    // half align
    if (alignment == LENGTH_HALF_WORD) return ((address % 2) == 0);
    // word align
    if (alignment == LENGTH_WORD) return ((address % 4) == 0);
  }
  return 0;
}


void store(Byte *memory, Address address, Alignment alignment, Word value, int check_align) {
  if ((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
    handle_invalid_write(address);
  }
  /* YOUR CODE HERE */
  switch(alignment) {
        case LENGTH_BYTE:
            memory[address] = (Byte) (value & 0x000000ff);
            break;
        case LENGTH_HALF_WORD:
            memory[address] = (Byte) (value & 0x000000ff);
            memory[address + 1] = (Byte) ((value & 0x0000ff00) >> 8);
            break;
        case LENGTH_WORD:
            memory[address] = (Byte) (value & 0x000000ff);
            memory[address + 1] = (Byte) ((value & 0x0000ff00) >> 8);
            memory[address + 2] = (Byte) ((value & 0x00ff0000) >> 16);
            memory[address + 3] = (Byte) ((value & 0xff000000) >> 24);
            break;
        default:
            return;
            break;
    }
}


Word load(Byte *memory, Address address, Alignment alignment, int check_align) {
  if ((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
    handle_invalid_read(address);
  }
  /* YOUR CODE HERE */
   uint32_t data = 0x00000000; // initialize our return value to zero
    switch(alignment) {
        case LENGTH_BYTE:
            data =  data | memory[address];
            break;
        case LENGTH_HALF_WORD:
            data =  data | memory[address];
            data =  data | (memory[address + 1] << 8);
            break;
        case LENGTH_WORD:
            data =  data | memory[address];
            data =  data | (memory[address + 1] << 8);
            data =  data | (memory[address + 2] << 16);
            data =  data | (memory[address + 3] << 24);
            break;
        default:
            break;
    }
    return data;
}
