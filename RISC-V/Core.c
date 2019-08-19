#include "Core.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;  
    int i;
    for (i = 0; i < 32; i++) { core->register_file[i] = 0;}
    for (i=0; i< 256; i++) {core->memmory[i] = 0;}
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
    control_unit(optype,instruction);

    
    //for R type, Read data 1 and 2;
    uint64_t read_data1 = core->register_file[rs1];
    uint64_t read_data2 = core->register_file[rs2]; 
    printf("rs_1\n", read_data1);
    printf("rs_2\n", read_data1);
    //if is negtive number, remove minus sign
    if ((control.imm & 0x800 >> 11) == 1)
    {
        imm = control.imm - 1;
        imm = ~imm;
        imm = imm & 0xFFF;

        
    }
    
    //ALU Unit Operation

    uint64_t data1 = read_data1;
    uint64_t data2 = Mux(control.ALUSrc,read_data1,read_data2);
    printf("rs_1: %u\n", read_data1);
    printf("rs_2: %u\n", read_data2);
    printf("ALU_In1: %u\n", data1);
    printf("ALU_In12: %u\n", data2);
    uint8_t ALUControlSignal = ALU_control(control.ALUOp,instruction);
    uint64_t ALU_result = ALU(data1,data2,ALUControlSignal);

    uint64_t ReadData;
    if (control.MemWrite){
        core->memmory[ALU_result] = read_data2;

    }
    if(control.MemRead){
        ReadData = core->memmory[ALU_result];
    }

    if(control.RegWrite){
        core->register_file[rd] = Mux(control.MemtoReg, ALU_result,ReadData);
    }
    
    
    
    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?
    
    unsigned PCcontrol = control.Branch && BranchControl(instruction,read_data1,read_data2);
    unsigned PCincrement = Mux(PCcontrol,4,(core->PC + (imm<<1)));

    core->PC += PCincrement;

    
    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}
void control_unit(unsigned optype,unsigned instruction)
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
            control.imm = 0; 
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
            control.imm = (instruction & 0b11111111111100000000000000000000) >> 20;
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
            control.imm = ((instruction & 0b11111110000000000000000000000000) >> 20) + ((instruction & 0b111110000000) >> 7);
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
            control.imm = ((instruction & 0b10000000000000000000000000000000) >> 20) + ((instruction & 0b10000000) << 3) + 
                          ((instruction & 0b1111110000000000000000000000000) >> 21) + ((instruction & 0b111100000000) >> 8);

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
            control.imm = (instruction & 0b11111111111100000000000000000000) >> 20;

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

unsigned func3(unsigned instruction)
{
    unsigned fn3 = 0;
    fn3 = instruction & 0xF80>>7;
    return fn3;
}

unsigned func7(unsigned instruction)
{
    unsigned fn7 = 0;
    fn7 = instruction & 0xFE000000>>25;
    return fn7;
}

uint64_t ALU(uint64_t data1, uint64_t data2, uint8_t ALU_Control_line)
{
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

    return result;
    
}


int BranchControl(unsigned instruction, unsigned rs1, unsigned rs2){

    int result;

    if(func3(instruction) == 0x0 && (rs1 == rs2))
    {
        result = 1;
    }
    else if(func3(instruction) && (rs1 != rs2))
    {
        result = 1;
    }
    else if(func3(instruction) && (rs1 < rs2))
    {
        result = 1;
    }
    else if(func3(instruction) && (rs1 >= rs2))
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    
    return result;
}
// Control


