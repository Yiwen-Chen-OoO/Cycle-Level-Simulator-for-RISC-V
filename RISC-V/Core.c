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
    
    
    /* (Step 2) Instruction Seperation
     *     opcode, rd, rs1, rs2
    */
    unsigned optype= instruction & 0x0000007F;    
    unsigned rs1 = instruction &   0x000F8000 >> 15;
    unsigned rs2 = instruction &   0x01F00000 >> 20;
    unsigned rd = instruction &    0x00000F80>>7;
    unsigned imm;
    
    /* (Step 3) Control Unit, take opcode as input
            Output 7 bool signal */
    control_unit(optype);

    
    //for R type, Read data 1 and 2;
    uint64_t read_data1 = core->register_file[rs1];
    uint64_t read_data2 = core->register_file[rs2];
    //if is negtive number, remove minus sign
    if ((instruction & 0x80000000 >> 31) == 1)
    {
        imm = instruction & 0x7FFFFFFF;
        imm = ~imm;
    }

    //ALU(read_data1,mux(control.ALUSrc,read_data2,imm),ALU_control);


    
    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?
    core->PC += 4;

    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}
void control_unit(unsigned optype)
{
    switch (optype)
    {
        case 0x33:
            //R type
            control.ALUSrc = false;
            control.MemtoReg = false;
            control.RegWrite = true;
            control.MemRead = false;
            control.MemWrite = false;
            control.Branch = false;
            control.ALUOp = 0b10;        
            break;
        case 0x3:
            // ld type I
            control.ALUSrc = true;
            control.MemtoReg = true;
            control.RegWrite = true;
            control.MemRead = true;
            control.MemWrite = false;
            control.Branch = false;
            control.ALUOp = 0b00; 
            break;
        case 0x23:
            // sd type S
            control.ALUSrc = true;
            control.MemtoReg = false;//don't care X
            control.RegWrite = false;
            control.MemRead = false;
            control.MemWrite = true;
            control.Branch = false;
            control.ALUOp = 0b00; 
            break;
        case 0x63:
            // beq type: SB
            control.ALUSrc = false;
            control.MemtoReg = false;//don't care X
            control.RegWrite = false;
            control.MemRead = false;
            control.MemWrite = false;
            control.Branch = true;
            control.ALUOp = 0b01; 
            break;

        case 0x13: 
            /* I type
             * ld: 0x03 up
             * addi, slli, srli,xori,ori,andi
             *  */
            control.ALUSrc = false;
            control.MemtoReg = false;
            control.RegWrite = false;
            control.MemRead = false;
            control.MemWrite = false;
            control.Branch = false;
            control.ALUOp = 0b00; 

            break;
        //jalr :I type x67/0
        //jal: UJ type x6F
    }
}
uint64_t Mux(bool signal, uint64_t data1, uint64_t data2)
{
    if (signal){
        return data2;
    } else {
        return data1;
    }
}

uint8_t ALU_control(uint8_t ALUOp, uint32_t instruction )
{
    if (ALUOp == 0b00){
        return 0b0010;
    } else if (ALUOp == 0b01){
        return 0b0110;
    } else {
        if (func3(instruction) == 0){
            if (func7(instruction)==0){
                return 0b0010;
            } else {
                return 0b0110;
            }
        } else if (func3(instruction)== 0x111){
            return 0;
        } else if (func3(instruction) == 0b110){
            return 1;
        }
    }
}

void ALU(uint64_t data1, uint64_t data2, uint8_t ALU_Control_line, uint64_t *ALU_zero, uint64_t *result)
{
    result = 0;
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
    
}



// Control


