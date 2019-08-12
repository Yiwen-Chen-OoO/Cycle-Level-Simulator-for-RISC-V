#include "Core.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;  
    uint8_t i;
    for (i = 0; i < 32; i++) { core->register_file[i] = 0;}

    return core;
}

// FIXME, implement this function
bool tickFunc(Core *core)
{
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    bool Branch, MemRead,MemtoReg,ALUOp,MemWrite,ALUSrc,RegWrite;
    
    // (Step 2) ...
    uint8_t optype= instruction & 0x0000007F;
    uint8_t rs1 = instruction &   0x000F8000 >> 12;
    uint8_t rs2 = instruction &   0x01F00000 >> 20;
    uint8_t wr = instruction &    0x00000F80 >> 4;

    // for R type, Read data 1 and 2;
    uint64_t read_data1 = core->register_file[rs1];
    uint64_t read_data2 = core->register_file[rs2];
    
    uint64_t ALU_result;
    ALU_result = ALU(read_data1,read_data2,&ALU_zero);

    
    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?!
    core->PC += 4;

    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}

uint64_t Mux(int signal, uint64_t data1, uint64_t data2)
{
    if (signal == 1){
        return data2;
    } else {
        return data1;
    }
}

uint64_t ALU(uint64_t data1, uint64_t data2, uint8_t ALU_Control_line, char *ALU_zero){
    uint64_t result = 0;
    if (ALU_Control_line == 0000){
        result = data1 & data2;
    } else if (ALU_Control_line == 0001){
        result = data1 | data2;
    } else if (ALU_Control_line == 0010){
        result = data1 + data2;
    }else if (ALU_Control_line == 0001){
        result = data1 - data2;
    }

    if (result == 0){
        ALU_zero = 1;
    } else {
        ALU_zero = 0;
    }
    return result;
}

// Control


