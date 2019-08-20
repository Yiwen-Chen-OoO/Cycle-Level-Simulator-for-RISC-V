#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define BOOL bool

struct CONTROL
{
    bool Branch, MemRead, MemtoReg, MemWrite,ALUSrc,RegWrite;
    uint8_t ALUOp;
    int64_t imm;
}control;

// pre-definition
struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Addr PC; // Keep track of program counter

    // TODO, define your components here
    // What else you need? Data memory? Register file?
    Instruction_Memory *instr_mem;
    int64_t register_file[32];
    int64_t memmory[256];
    // TODO, simulation function
    bool (*tick)(Core *core);

    
}Core;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);
void control_unit(unsigned optype,unsigned instruction);
int64_t Mux(bool signal, int64_t data1, int64_t data2);
uint8_t ALU_control(uint8_t ALUOp, uint32_t instruction );
unsigned func3(unsigned instruction);
unsigned func7(unsigned instruction);
int ALU(int64_t data1, int64_t data2, uint8_t ALU_Control_line);
int BranchControl(unsigned instruction, unsigned rs1, unsigned rs2);
unsigned getImm(unsigned instruction);

#endif
