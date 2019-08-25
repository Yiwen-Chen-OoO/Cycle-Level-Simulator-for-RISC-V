/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-03 09:03:28
 * @LastEditTime: 2019-08-19 15:53:16
 * @LastEditors: Please set LastEditors
 */
#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Registers.h"
#include "Instruction.h"
#include <inttypes.h>


#define BOOL bool
#define MEM_SIZE 1024

struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Addr PC;  // Keep track of program counter

    // TODO, define your components here
    // What else you need? Data memory? Register file?
    Instruction_Memory *instr_mem;
    int Data_Memory[MEM_SIZE];
    int Register[NUM_OF_REGS];

    // TODO, simulation function
    bool (*tick)(Core *core);
} Core;

typedef struct ControlSignal
{
    uint8_t ALUSrc;
    uint8_t MemtoReg;
    uint8_t RegWrite;
    uint8_t MemRead;
    uint8_t MemWrite;
    uint8_t beq;
    uint8_t bne;
    uint8_t blt;
    uint8_t bge;
    uint8_t jal;
    uint8_t ALUOp;
} Ctl;

// IF_ID Register
typedef struct IF_ID
{
    unsigned instruction;
    Addr PC;
    int valid;
} IF_ID;

//ID_EX Register
typedef struct ID_EX
{
    Addr PC;

    int Imm;
    int rs_1;
    int rs_2;
    int rd;
    uint8_t MemtoReg;
    uint8_t RegWrite;
    uint8_t MemRead;
    uint8_t MemWrite;
    uint8_t beq;
    uint8_t bne;
    uint8_t blt;
    uint8_t bge;
    uint8_t jal;
    uint8_t ALUCtl;
    uint8_t AluSrc;
    int valid;
} ID_EX;

//EX_MEM Register
typedef struct EX_MEM
{
    int rs_2;
    int rs_2_data;
    int res;
    int rd;
    uint8_t MemtoReg;
    uint8_t RegWrite;
    uint8_t MemRead;
    uint8_t MemWrite;
    int valid;
} EX_MEM;

//MEM_WB Register
typedef struct MEM_WB
{
    int rd;
    int written_data;
    uint8_t MemtoReg;
    uint8_t RegWrite;
    int valid;
} MEM_WB;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);
int ALU(int op1, int op2, int *res, uint8_t AluCtl);
uint8_t AluControl(uint8_t AluOp, uint8_t func3, uint8_t func7);
void Control(Ctl *control, unsigned opcode, unsigned func3);
int getImm(unsigned instruction);

#endif
