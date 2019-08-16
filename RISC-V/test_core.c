#include "Core.h"

struct CONTROL{
    unsigned Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite;
}control;

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core)); // memory locate in computer ram
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;

    //core->register_file[0] = 0; // for testing
    //core->reigster_file[7]= 100; // for testing

    return core;
}

unsigned mux_function(unsigned sl, unsigned input1, unsigned input2)
{
    unsigned result;

    if (sl == 0)
    {
        result = input1;
    }
    else
    {
        result = input2;
    }
    
    return result;
}

unsigned ALU(unsigned ReadData1, unsigned ReadData2, unsigned ALUCONTROLSIGNAL)
{
    unsigned ALUresult;

    switch (ALUCONTROLSIGNAL)
    {
        case 0b0000:
            ALUresult = ReadData1 & ReadData2;
            break;

        case 0b0001:
            ALUresult = ReadData1 | ReadData2;
            break;

        case 0b0010:
            ALUresult = ReadData1 + ReadData2;
            break;

        case 0b0110:
            ALUresult = ReadData1 - ReadData2;
            break;
    }

    return ALUresult;
}

unsigned ALUcontrol(unsigned ALUop, unsigned func)
{
    unsigned ALUCONTROLSIGNAL;

    if (ALUop == 0b00 && func == 0b0000)
    {
        ALUCONTROLSIGNAL = 0b0010;
    }
    else if (ALUop == 0b01 && func == 0b0000)
    {
        ALUCONTROLSIGNAL = 0b0110;
    }
    else if (ALUop == 0b10 && func == 0b0000)
    {
        ALUCONTROLSIGNAL = 0b0010;
    }
    else if (ALUop == 0b10 && func == 0b1000)
    {
        ALUCONTROLSIGNAL = 0b0110;
    }
    else if (ALUop == 0b10 && func == 0b0111)
    {
        ALUCONTROLSIGNAL = 0b0000;
    }
    else if (ALUop == 0b10 && func == 0b0110)
    {
        ALUCONTROLSIGNAL = 0b0001;
    }

    return ALUCONTROLSIGNAL;
}

void control_uint(unsigned opcode)
{

    switch (opcode)
    {
        case 51: // R type
            control.Branch = 0;
            control.MemRead = 0;
            control.MemtoReg = 0;
            control.ALUOp = 0b10;
            control.MemWrite = 0;
            control.ALUSrc = 0;
            control.RegWrite = 1;

            break;
        
        case 0x3: // ld
            control.Branch = 0;
            control.MemRead = 1;
            control.MemtoReg = 1;
            control.ALUOp = 0b00;
            control.MemWrite = 0;
            control.ALUSrc = 1;
            control.RegWrite = 1;
            
            break;
        
        case 0x13: // I type
            control.Branch = 0;
            control.MemRead = 0;
            control.MemtoReg = 0;
            control.ALUOp = 0b00;
            control.MemWrite = 0;
            control.ALUSrc = 0;
            control.RegWrite = 0;

            break;
        
        case 0x23: // sd S type
            control.Branch = 0;
            control.MemRead = 0;
            control.MemtoReg = 0; // X Don't care
            control.ALUOp = 0b00;
            control.MemWrite = 1;
            control.ALUSrc = 1;
            control.RegWrite = 0;

            break;

        case 0x63: // SB type
            control.Branch = 1;
            control.MemRead = 0;
            control.MemtoReg = 0; // X Don't care
            control.ALUOp = 0b01;
            control.MemWrite = 0;
            control.ALUSrc = 0;
            control.RegWrite = 0;

            break;
    }

}

// FIXME, implement this function
bool tickFunc(Core *core)
{
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction; // return 32-bit instruciton code
    
    // (Step 2) Get opcode, rs1, rs2, rd
    unsigned opcode = instruction & 0b1111111; // index of opcode
    unsigned rd = (instruction & 0b111110000000) >> 7; // index of rd
    unsigned rs_1 = (instruction & 0b11111000000000000000) >> 15; // index of rs 1
    unsigned rs_2 = (instruction & 0b1111100000000000000000000) >> 20; // index of rs 2
    unsigned imm;
    unsigned ReadData1, ReadData2;

    // (Step 3) Control
    control_uint(opcode); // input opcode and output from the control function
    
    // (Step 4) Registers
    unsigned rs_1_value = core->register_file[rs_1]; // Read from rs1
    unsigned rs_2_value = core->register_file[rs_2]; // Read from rs2

    // (Step 5) Imm Gen
    ReadData1 = rs_1_value;
    
    if (((instruction & 0b10000000000000000000000000000000) >> 31) == 1)
    {
        imm = instruction - 0b1;
        imm = ~imm; // 2's complement
        ReadData2 = mux_function(control.ALUSrc, rs_2_value, imm); // first mux
    }
    else
    {
        ReadData2 = mux_function(control.ALUSrc, rs_2_value, imm); // first mux
    }
    
    // (Step 6) ALU Control
    unsigned func = ((instruction & 1073741824) >> 27) + ((instruction & 0b111000000000000) >> 12);
    unsigned ALUCONTROLSIGNAL = ALUcontrol(control.ALUOp, func);

    // (Step 7) ALU
    unsigned ALUresult;
    ALUresult = ALU(ReadData1, ReadData2, ALUCONTROLSIGNAL);


    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?!
    unsigned PCcontrol = control.Branch && 0;
    unsigned PCincrement = mux_function(PCcontrol, 4, (core->PC + (imm << 1))); // Second mux, Shift left 1
    core->PC += PCincrement;

    // increase clk after each instruction code executed
    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}
