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
    
    // (Step 2) ...
    uint8_t optype= instruction & 0x0000007F;
    uint8_t rs1 = instruction &   0x000F8000;
    uint8_t rs2 = instruction &   0x01F00000;
    uint8_t wr = instruction &    0x00000F80;
    
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

// int ALU
// Control


