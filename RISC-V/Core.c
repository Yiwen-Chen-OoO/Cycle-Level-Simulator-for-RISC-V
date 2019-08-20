#include "Core.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>



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
    //test 1
    core->register_file[1] = 0;
    core->register_file[2] = 10;
    core->register_file[3] = 15;
    core->register_file[4] = 20;
    core->register_file[5] = 30;
    core->register_file[6] = 35;

    core->memmory[40] = 5;
    core->memmory[48] = 63;
    
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
    unsigned rs1 = (instruction &   0xfe000) >> 15;
    unsigned rs2 = (instruction & 0x1f00000) >> 20;
    unsigned rd = (instruction & 0xf80)>>7;
    unsigned imm;
    
    /* (Step 3) Control Unit, take opcode as input
            Output 7 bool signal */
    imm = getImm(instruction);

    control_unit(optype,instruction);
    control.imm = imm;
    
    //for R type, Read data 1 and 2;ls
    int64_t read_data1 = core->register_file[rs1];
    int64_t read_data2 = core->register_file[rs2]; 

    //if is negtive number, remove minus sign
    if ((control.imm & 0x800 >> 11) == 1)
    {
        imm = control.imm - 1;
        imm = ~imm;
        imm = imm & 0xFFF;        
    }
    
    //ALU Unit Operation

    int64_t data1 = read_data1;
    int64_t data2 = Mux(control.ALUSrc,read_data2,control.imm);
    uint8_t ALUControlSignal = ALU_control(control.ALUOp,instruction);
    int ALU_result = ALU(data1,data2,ALUControlSignal);
    printf("r1 %u \n", rs1);
    printf("r2 %u \n", rs2);
    printf("rd %u \n", rd);
    printf("immi %"PRId64" \n", control.imm);
    printf("data1 %"PRId64"\n", data1);
    printf("data2 %"PRId64"\n", data2);
    printf("ALUOpl %"PRIu8"\n",control.ALUOp);
    printf("ALUSignal %"PRIu8"\n",ALUControlSignal);
    printf("ALU_result: %d\n", ALU_result);
    int64_t ReadData;
    if (control.MemWrite){
        core->memmory[ALU_result] = read_data2;

    }
    if(control.MemRead){
        ReadData = core->memmory[ALU_result];
        printf("MemRead:");
        printf("%"PRId64"\n",core->memmory[ALU_result]);
    }

    if(control.RegWrite){
        core->register_file[rd] = Mux(control.MemtoReg, ALU_result,ReadData);
        printf("RegWrite:");
        printf("%"PRId64"\n",core->register_file[rd]);
    }
    
    printf("ctrl_immi %"PRId64" \n", control.imm);
    int i;
    for (i=0;i<12;++i){
    printf("rst@%i %"PRId64" \n",i,core->register_file[i]);
    }
    for (i=40;i<50;++i){
    printf("mem@%i %"PRId64" \n",i,core->memmory[i]);
    }
    printf("===========================================");
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

unsigned getImm(unsigned instruction)
{

    //I-Type
    unsigned int opcode = instruction & 0b1111111;
    unsigned Imm;
    if (opcode == 0b0000011 || opcode == 0b0010011)
    {
        Imm = instruction >> 20;
        if (Imm & 0b100000000000)
            Imm |= 0xfffff000;
    }
    // S-Type
    else if (opcode == 0b0100011)
    {
        Imm = ((instruction >> 25) << 5) + ((instruction & 0x00000f80) >> 7);
        if (Imm & 0b100000000000)
            Imm |= 0xfffff000;
    }
    // SB-Type
    else if (opcode == 0b1100011)
    {
        unsigned imm1, imm2;
        imm1 = instruction >> 25;
        imm2 = ((instruction & 0x00000f80) >> 7);
        Imm = 0;
        Imm |= ((imm2 & 1) << 11);
        Imm |= (imm2 & 0b11110);
        Imm |= ((imm1 & 0b1000000) << 6);
        Imm |= ((imm1 & 0b0111111) << 5);

        if (Imm & 0x1000)
            Imm |= 0xffffe000;
    }
    // UJ-Type
    else if (opcode == 0b1101111)
    {
        unsigned int imm = instruction >> 12;
        Imm = 0;
        Imm |= ((imm & 0b11111111) << 12);
        Imm |= ((imm & 0x100) << 3);
        Imm |= ((imm & 0x7fe00) >> 8);
        Imm |= ((imm & 0x80000) << 1);

        if (Imm & 0x100000)
            Imm |= 0xffe00000;
    }
    return Imm;
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
            control.imm = (instruction & 0b111110000000) >> 7;
            control.imm = control.imm|((instruction) & 0xFE000000>>25<<7);

            break;
        case 0x23:
            // sd type S
            control.ALUSrc = true;
            control.MemtoReg = false;//don't care X
            control.RegWrite = false;
            control.MemRead = true;
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
            control.ALUSrc = true;
            control.MemtoReg = false;
            control.RegWrite = true;
            control.MemRead = false;
            control.MemWrite = false;
            control.Branch = false;
            control.ALUOp = 0b10; 
            control.imm = (instruction & 0b11111111111100000000000000000000) >> 20;

            break;
        //jalr :I type x67/0
        //jal: UJ type x6F
    }
}
int64_t Mux(bool signal, int64_t data1, int64_t data2)
{
    if (signal){
        return data2;
    } else {
        return data1;
    }
}

uint8_t ALU_control(uint8_t ALUOp, uint32_t instruction )
{
    uint8_t fn3 = func3(instruction);
    printf("func3 %"PRIu8"\n",fn3);
    switch (ALUOp){
        // ld and sd
    case 0b00:
        return 0b0010;

    case 0b01:
        // beq and bne
        if (fn3 == 0 || fn3 == 0b001)
            return 0b0110;

        // blt, bge, bltu and bgeu
        if (fn3 >= 0b100 && fn3 <= 0b111)
            return 0b0111;
    //  R-type
    case 0b10:
        switch (fn3)
        {
        case 0:
            // add
            if (func7(instruction) == 0)
                return 0b0010;
            // sub
            if (func7(instruction) == 0b0100000)
                return 0b0110;
        // sll
        case 0b001:
            return 0b0011;
        // slt
        case 0b010:
            return 0b0111;
        // xor
        case 0b100:
            return 0b0101;
        // srl
        case 0b101:
            return 0b0100;
        // or
        case 0b110:
            return 0b0001;
        // and
        case 0b111:
            return 0;
        }
    default:
        return -1;
    }

}

unsigned func3(unsigned instruction)
{
    unsigned fn3 = 0;
    fn3 = (instruction & 0x7000)>>12;
    return fn3;
}

unsigned func7(unsigned instruction)
{
    unsigned fn7 = 0;
    fn7 = instruction >>25;
    return fn7;
}

int ALU(int64_t data1, int64_t data2, uint8_t ALU_Control_line)
{
    int result = 0;
    if (ALU_Control_line == 0b0000){
        result = data1 & data2;
    } else if (ALU_Control_line == 0b0001){
        result = data1 | data2;
    } else if (ALU_Control_line == 0b0010){
        result = data1 + data2;
    }else if (ALU_Control_line == 0b0110){
        result = data1 - data2;
    }else if (ALU_Control_line == 0b0011){
        result = data1 << data2;
    }else if (ALU_Control_line == 0b0100){
        result = data1 >> data2;
    }else if (ALU_Control_line == 0b0101){
        result = data1 ^ data2;
    }else if (ALU_Control_line == 0b0111){
        result = (data1 < data2 ? 1 : 0);
    }else{
        result = (data1 == data2 ? 1 : 0);
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


