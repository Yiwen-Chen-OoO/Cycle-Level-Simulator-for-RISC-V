/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-03 09:03:28
 * @LastEditTime: 2019-08-19 16:04:11
 * @LastEditors: Please set LastEditors
 */
#include "Core.h"
#include "Registers.h"

// declare pipline registers and initialize them
IF_ID IF_ID_Reg;
ID_EX ID_EX_Reg;
EX_MEM EX_MEM_Reg;
MEM_WB MEM_WB_Reg;
Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;

    memset(core->Register, 0, NUM_OF_REGS * sizeof(int));
    memset(core->Data_Memory, 0, MEM_SIZE * sizeof(int));

    core->Register[1] = 8;
    core->Register[5] = 4;
    core->Register[3] = 4;
    ///////////////////////////////////////////////////////////////////////////
    // for the third simulation

    // first one for task_1
    core->Register[1] = 8;
    core->Register[3] = -15;
    core->Register[5] = 255;
    core->Register[6] = 1023;

    printf("======================================\n");
    printf("Initialization...\n\n\n");
    printf("Registers:\n");
    printf("Register x1:    %d\n", core->Register[1]);
    printf("Register x3:    %d\n", core->Register[3]);
    printf("Register x5:    %d\n", core->Register[5]);
    printf("Register x6:    %d\n\n", core->Register[6]);
    printf("======================================\n");
    // second one for task_3

    // core->Register[5] = 26;
    // core->Register[6] = -27;

    // core->Data_Memory[20] = 100;
    // printf("======================================\n");
    // printf("Initialization...\n\n\n");
    // printf("Registers:\n");

    // printf("Register x5:    %d\n", core->Register[5]);
    // printf("Register x6:    %d\n\n", core->Register[6]);
    // printf("Memory:\n");
    // printf("Address 100:    %d\n", core->Data_Memory[100]);

    // printf("======================================\n");
    ///////////////////////////////////////////////////////////////////////////

    IF_ID_Reg.valid = 0;
    ID_EX_Reg.valid = 0;
    EX_MEM_Reg.valid = 0;
    MEM_WB_Reg.valid = 0;

    return core;
}

void data_hazard_detection(int *forwardA, int *forwardB)
{
    // data hazard

    // EX hazard
    if ((EX_MEM_Reg.RegWrite || EX_MEM_Reg.MemtoReg) && EX_MEM_Reg.rd && EX_MEM_Reg.rd == ID_EX_Reg.rs_1)
    {
        *forwardA = 0b10;
        printf("register %d has EX hazard\n", ID_EX_Reg.rs_1);
    }
    if ((EX_MEM_Reg.RegWrite || EX_MEM_Reg.MemtoReg) && EX_MEM_Reg.rd && EX_MEM_Reg.rd == ID_EX_Reg.rs_2)
    {
        *forwardB = 0b10;
        printf("register %d has EX hazard\n", ID_EX_Reg.rs_2);
    }

    // MEM hazard
    if ((MEM_WB_Reg.RegWrite || MEM_WB_Reg.MemtoReg) && MEM_WB_Reg.rd && MEM_WB_Reg.rd == ID_EX_Reg.rs_1)
    {
        *forwardA = 0b01;
        printf("register %d has MEM hazard\n", ID_EX_Reg.rs_1);
    }
    if ((MEM_WB_Reg.RegWrite || MEM_WB_Reg.MemtoReg) && MEM_WB_Reg.rd && MEM_WB_Reg.rd == ID_EX_Reg.rs_2)
    {
        *forwardB = 0b01;
        printf("register %d has MEM hazard\n", ID_EX_Reg.rs_2);
    }
}

// FIXME, implement this function
bool tickFunc(Core *core)
{

    static int valid = 1;

    // declare intermediate variable of each stage
    // IF
    unsigned instruction;

    // ID
    unsigned pipeline_instruction;
    Addr IF_ID_pipeline_PC;
    Ctl control;
    uint8_t AluCtl;
    int Imm;
    uint8_t rd, rs_1, rs_2;

    // EXE
    int EXE_MEM_pipeline_rs_2;
    int rs_2_data;
    int res;
    int EXE_MEM_pipeline_rd;
    uint8_t EXE_MEM_MemtoReg;
    uint8_t EXE_MEM_RegWrite;
    uint8_t EXE_MEM_MemRead;
    uint8_t EXE_MEM_MemWrite;

    // MEM
    int MEM_WB_pipeline_rd;
    int written_data;
    uint8_t MEM_WB_MemtoReg;
    uint8_t MEM_WB_RegWrite;
    printf("****************************************************************\n");
    printf("======================================\n");
    // int stop_next_1 = 1, stop_next_2 = 1, stop_next_3 = 1, stop_next_4 = 1;
    // int start_next_1 = 0, start_next_2 = 0, start_next_3 = 0, start_next_4 = 0;

    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    if (valid)
    {
        instruction = core->instr_mem->instructions[core->PC / 4].instruction;
        core->PC += 4;
        printf("IF stage:\n\n");
        printf("PC:    %llu\n", core->PC);
        printf("machine code:    ");
        unsigned tmp = instruction;
        int cnt = 0;
        while (cnt < 32)
        {
            printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
            ++cnt;
        }
        printf("\n");
        printf("======================================\n");
        // IF_ID_Reg.valid = start_next_1;
        // start_next_1 = 1;
        // printf("1 ");
    }

    // (Step 2) ID(Instruction Decoding)
    if (IF_ID_Reg.valid)
    {
        pipeline_instruction = IF_ID_Reg.instruction;
        IF_ID_pipeline_PC = IF_ID_Reg.PC;

        // decode

        unsigned opcode, func3, func7;
        opcode = pipeline_instruction & 0x7f;
        func3 = (pipeline_instruction & 0x7000) >> 12;
        func7 = pipeline_instruction >> 25;

        // registers
        rd = (pipeline_instruction & 0xf80) >> 7;
        rs_1 = (pipeline_instruction & 0xfe000) >> 15;
        rs_2 = (pipeline_instruction & 0x1f00000) >> 20;
        // printf("rd:   %d\n", rd);
        // Imm

        Imm = getImm(pipeline_instruction);
        // printf("Imm: %d , \n", Imm);
        // update control signal
        Control(&control, opcode, func3);

        AluCtl = AluControl(control.ALUOp, func3, func7);

        // printf("op1:   %d\n", op1);
        // printf("op2:   %d\n", op2);
        // printf("AluCtl:   %d\n", AluCtl);
        // printf("ALUCtl: %d\n", AluCtl);

        // branch comparator
        int zero = core->Register[rs_1] == core->Register[rs_2] ? 1 : 0;
        // branch
        if (control.beq && zero)
        {
            core->PC = IF_ID_pipeline_PC - 4 + Imm;
            printf("Here is a control hazard\n");
        }

        else if (control.bne && !zero)
        {
            core->PC = IF_ID_pipeline_PC - 4 + Imm;
            printf("Here is a control hazard\n");
        }

        // ID_EX_Reg.valid = start_next_2;
        // start_next_2 = 1;
        // printf("2 ");
        printf("ID stage:\n\n");
        printf("IF_ID pipeline register: \n");
        printf("IF_ID--->instruction:    ");
        unsigned tmp = IF_ID_Reg.instruction;
        int cnt = 0;
        while (cnt < 32)
        {
            printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
            ++cnt;
        }
        printf("\n");
        printf("IF_ID--->PC:    %llu\n", IF_ID_Reg.PC);
        printf("======================================\n");
    }

    // (step 3) EX(Execute)
    if (ID_EX_Reg.valid)
    {
        printf("EX stage:\n\n");
        printf("ID_EX pipeline register: \n");
        printf("ID_EX--->PC:    %llu\n", ID_EX_Reg.PC);
        printf("ID_EX--->Imm:    %d\n", ID_EX_Reg.Imm);
        printf("ID_EX--->rs1:    %d\n", ID_EX_Reg.rs_1);
        printf("ID_EX--->rs2:    %d\n", ID_EX_Reg.rs_2);
        printf("ID_EX--->rd:    %d\n", ID_EX_Reg.rd);
        printf("ID_EX--->MemtoReg:    %d\n", ID_EX_Reg.MemtoReg);
        printf("ID_EX--->RegWrite:    %d\n", ID_EX_Reg.RegWrite);
        printf("ID_EX--->MemRead:    %d\n", ID_EX_Reg.MemRead);
        printf("ID_EX--->MemWrite:    %d\n", ID_EX_Reg.MemWrite);
        printf("ID_EX--->ALUCtl:    %d\n", ID_EX_Reg.ALUCtl);
        printf("ID_EX--->AluSrc:    %u\n", ID_EX_Reg.AluSrc);
        printf("ID_EX--->beq:    %d\n", ID_EX_Reg.beq);
        printf("ID_EX--->bne:    %d\n", ID_EX_Reg.bne);
        printf("ID_EX--->blt:    %d\n", ID_EX_Reg.blt);
        printf("ID_EX--->bge:    %d\n", ID_EX_Reg.bge);
        printf("ID_EX--->jal:    %d\n", ID_EX_Reg.jal);
        printf("======================================\n");
        int forwardA = 0, forwardB = 0;
        data_hazard_detection(&forwardA, &forwardB);

        int pipeline_op1;
        int pipeline_op2;
        int pipeline_PC = ID_EX_Reg.PC;
        uint8_t pipeline_AluCtl = ID_EX_Reg.ALUCtl;
        EXE_MEM_pipeline_rs_2 = ID_EX_Reg.rs_2;
        EXE_MEM_pipeline_rd = ID_EX_Reg.rd;
        EXE_MEM_MemtoReg = ID_EX_Reg.MemtoReg;
        EXE_MEM_RegWrite = ID_EX_Reg.RegWrite;
        EXE_MEM_MemRead = ID_EX_Reg.MemRead;
        EXE_MEM_MemWrite = ID_EX_Reg.MemWrite;

        // printf("EXE_MEM_pipeline_rd:    %d\n", EXE_MEM_pipeline_rd);

        // forward unit

        // for the first op
        switch (forwardA)
        {
        case 0b00:
            pipeline_op1 = core->Register[ID_EX_Reg.rs_1];
            break;
        case 0b10:
            pipeline_op1 = EX_MEM_Reg.res;
            break;
        case 0b01:
            pipeline_op1 = MEM_WB_Reg.written_data;
            break;

        default:
            break;
        }

        rs_2_data = core->Register[ID_EX_Reg.rs_2];

        // for the second op
        switch (forwardB)
        {
        case 0b00:
            pipeline_op2 = ID_EX_Reg.AluSrc ? ID_EX_Reg.Imm : core->Register[ID_EX_Reg.rs_2];
            break;
        case 0b10:
            pipeline_op2 = EX_MEM_Reg.res;
            rs_2_data = EX_MEM_Reg.res;
            break;
        case 0b01:
            pipeline_op2 = MEM_WB_Reg.written_data;
            rs_2_data = MEM_WB_Reg.written_data;
            break;

        default:
            break;
        }
        printf("rs_2_data: %d\n", rs_2_data);

        unsigned zero;
        // printf("rd:   %d\n", EX_MEM_Reg.rd);
        // printf("pipeline_op1:  %d\n", pipeline_op1);
        // printf("pipeline_op2:  %d\n", pipeline_op2);
        // printf("pipeline_AluCtl:   %d\n", pipeline_AluCtl);
        zero = ALU(pipeline_op1, pipeline_op2, &res, pipeline_AluCtl);
        // printf("res %d\n", res);
        printf("res: %d\n", res);
        // update PC

        if (ID_EX_Reg.blt && res)
            core->PC = pipeline_PC - 4 + ID_EX_Reg.Imm;
        else if (ID_EX_Reg.bge && !res)
            core->PC = pipeline_PC - 4 + ID_EX_Reg.Imm;
        else if (ID_EX_Reg.jal)
        {
            core->Register[ID_EX_Reg.rd] = pipeline_PC;
            core->PC = pipeline_PC - 4 + ID_EX_Reg.Imm;
        }

        // printf("3 ");
    }

    // (step 4) MEM(Memory Access)
    if (EX_MEM_Reg.valid)
    {
        printf("MEM stage:\n\n");
        printf("EX_MEM pipeline register:\n");
        printf("EX_MEM--->rs_2:    %d\n", EX_MEM_Reg.rs_2);
        printf("EX_MEM--->res:    %d\n", EX_MEM_Reg.res);
        printf("EX_MEM--->rd:    %d\n", EX_MEM_Reg.rd);
        printf("EX_MEM--->MemtoReg:    %d\n", EX_MEM_Reg.MemtoReg);
        printf("EX_MEM--->RegWrite:    %d\n", EX_MEM_Reg.RegWrite);
        printf("EX_MEM--->MemRead:    %d\n", EX_MEM_Reg.MemRead);
        printf("EX_MEM--->MemWrite:    %d\n", EX_MEM_Reg.MemWrite);
        printf("======================================\n");
        MEM_WB_pipeline_rd = EX_MEM_Reg.rd;
        MEM_WB_MemtoReg = EX_MEM_Reg.MemtoReg;
        MEM_WB_RegWrite = EX_MEM_Reg.RegWrite;
        if (EX_MEM_Reg.MemWrite)
        {
            core->Data_Memory[EX_MEM_Reg.res] = EX_MEM_Reg.rs_2_data;
        }

        if (EX_MEM_Reg.MemRead)
        {
            written_data = core->Data_Memory[EX_MEM_Reg.res];
            // printf("mem_data: %d\n", written_data);
        }
        if (EX_MEM_Reg.RegWrite)
        {
            written_data = EX_MEM_Reg.res;
        }
        // printf("MEM_WB_pipeline_rd:    %d\n", MEM_WB_pipeline_rd);
        // printf("4 ");
    }

    // (step 5) WB(Write Back)
    if (MEM_WB_Reg.valid)
    {
        printf("WB stage:\n\n");
        printf("MEM_WB pipeline register:\n");
        printf("MEM_WB--->rd:    %d\n", MEM_WB_Reg.rd);
        printf("MEM_WB--->written_data:    %d\n", MEM_WB_Reg.written_data);
        printf("MEM_WB--->MemtoReg:    %d\n", MEM_WB_Reg.MemtoReg);
        printf("MEM_WB--->RegWrite:    %d\n", MEM_WB_Reg.RegWrite);
        printf("======================================\n");
        if (MEM_WB_Reg.MemtoReg && MEM_WB_Reg.rd != 0)
        {
            // printf("rd: %d\n", MEM_WB_Reg.rd);

            core->Register[MEM_WB_Reg.rd] = MEM_WB_Reg.written_data;
        }

        if (MEM_WB_Reg.RegWrite && MEM_WB_Reg.rd != 0)
        {
            core->Register[MEM_WB_Reg.rd] = MEM_WB_Reg.written_data;
            // printf("data:    %d\n", MEM_WB_Reg.written_data);
            // printf("regs:    %d\n", MEM_WB_Reg.rd);
        }
        // printf("5 ");
    }
    // printf("\n");

    // update pipeline registers

    // IF_ID Register
    IF_ID_Reg.PC = core->PC;
    IF_ID_Reg.instruction = instruction;

    // ID_EXE Register
    ID_EX_Reg.PC = IF_ID_pipeline_PC;
    ID_EX_Reg.ALUCtl = AluCtl;
    ID_EX_Reg.AluSrc = control.ALUSrc;
    ID_EX_Reg.beq = control.beq;
    ID_EX_Reg.bge = control.bge;
    ID_EX_Reg.blt = control.blt;
    ID_EX_Reg.bne = control.bne;
    ID_EX_Reg.Imm = Imm;
    ID_EX_Reg.jal = control.jal;
    ID_EX_Reg.MemRead = control.MemRead;
    ID_EX_Reg.MemtoReg = control.MemtoReg;
    ID_EX_Reg.MemWrite = control.MemWrite;
    ID_EX_Reg.rd = rd;
    ID_EX_Reg.RegWrite = control.RegWrite;
    ID_EX_Reg.rs_2 = rs_2;
    ID_EX_Reg.rs_1 = rs_1;

    // EXE_MEM Register
    EX_MEM_Reg.rs_2 = EXE_MEM_pipeline_rs_2;
    EX_MEM_Reg.rs_2_data = rs_2_data;
    EX_MEM_Reg.res = res;
    EX_MEM_Reg.rd = EXE_MEM_pipeline_rd;
    EX_MEM_Reg.MemtoReg = EXE_MEM_MemtoReg;
    EX_MEM_Reg.RegWrite = EXE_MEM_RegWrite;
    EX_MEM_Reg.MemRead = EXE_MEM_MemRead;
    EX_MEM_Reg.MemWrite = EXE_MEM_MemWrite;

    // MEM_WB Register
    MEM_WB_Reg.rd = MEM_WB_pipeline_rd;
    MEM_WB_Reg.written_data = written_data;
    MEM_WB_Reg.MemtoReg = MEM_WB_MemtoReg;
    MEM_WB_Reg.RegWrite = MEM_WB_RegWrite;

    // update state
    MEM_WB_Reg.valid = EX_MEM_Reg.valid;
    EX_MEM_Reg.valid = ID_EX_Reg.valid;
    ID_EX_Reg.valid = IF_ID_Reg.valid;
    IF_ID_Reg.valid = valid;

    ++core->clk;
    /////////////////////////////////////////////////////////////////////////
    // first one for task_1
    printf("CPU registers:\n\n");
    printf("Register x1:    %d\n", core->Register[1]);
    printf("Register x2:    %d\n", core->Register[2]);
    printf("Register x3:    %d\n", core->Register[3]);
    printf("Register x5:    %d\n", core->Register[5]);
    printf("Register x6:    %d\n", core->Register[6]);
    printf("Register x12:    %d\n", core->Register[12]);
    printf("Register x13:    %d\n", core->Register[13]);
    printf("Register x14:    %d\n", core->Register[14]);
    printf("\nData Memory: \n\n");
    printf("Address %d:    %d\n", 100 + core->Register[2], core->Data_Memory[100 + core->Register[2]]);

    printf("======================================\n");

    // second one for task_2
    // printf("CPU registers:\n\n");
    // printf("Register x1:    %d\n", core->Register[1]);
    // printf("Register x2:    %d\n", core->Register[2]);
    // printf("Register x4:    %d\n", core->Register[4]);
    // printf("Register x5:    %d\n", core->Register[5]);
    // printf("Register x6:    %d\n", core->Register[6]);
    // printf("Register x8:    %d\n", core->Register[8]);
    // printf("Register x9:    %d\n", core->Register[9]);

    // printf("======================================\n");
    /////////////////////////////////////////////////////////////////////////
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        valid = 0;
    }
    if (!valid && !IF_ID_Reg.valid && !ID_EX_Reg.valid && !EX_MEM_Reg.valid && !MEM_WB_Reg.valid)
        return false;
    return true;
}

// ALU
int ALU(int op1, int op2, int *res, uint8_t AluCtl)
{
    switch (AluCtl)
    {
    // add
    case 0b0010:
        *res = op1 + op2;
        break;

    // substract
    case 0b0110:
        *res = op1 - op2;
        break;

    // and
    case 0b0000:
        *res = op1 & op2;
        break;

    // or
    case 0b0001:
        *res = op1 | op2;
        break;

    // shift left
    case 0b0011:
        *res = op1 << op2;
        break;

    //shift right
    case 0b0100:
        *res = op1 >> op2;
        break;

    // xor
    case 0b0101:
        *res = op1 ^ op2;
        break;

    // slt
    case 0b0111:
        *res = op1 < op2 ? 1 : 0;
    }
    return op1 == op2 ? 1 : 0;
}

uint8_t AluControl(uint8_t AluOp, u_int8_t func3, u_int8_t func7)
{
    switch (AluOp)
    {
    // ld and sd
    case 0b00:
        return 0b0010;

    case 0b01:
        // beq and bne
        if (func3 == 0 || func3 == 0b001)
            return 0b0110;

        // blt, bge, bltu and bgeu
        if (func3 >= 0b100 && func3 <= 0b111)
            return 0b0111;
    //  R-type
    case 0b10:
        switch (func3)
        {
        case 0:
            // add
            if (func7 == 0)
                return 0b0010;
            // sub
            if (func7 == 0b0100000)
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

void Control(Ctl *control, unsigned opcode, unsigned func3)
{
    // R-Type
    if (opcode == 0b0110011)
    {
        control->ALUSrc = 0;
        control->MemtoReg = 0;
        control->RegWrite = 1;
        control->MemRead = 0;
        control->MemWrite = 0;
        control->beq = 0;
        control->bne = 0;
        control->blt = 0;
        control->bge = 0;
        control->jal = 0;
        control->ALUOp = 0b10;
    }
    // ld
    else if (opcode == 0b0000011)
    {
        control->ALUSrc = 1;
        control->MemtoReg = 1;
        control->RegWrite = 0;
        control->MemRead = 1;
        control->MemWrite = 0;
        control->beq = 0;
        control->bne = 0;
        control->blt = 0;
        control->bge = 0;
        control->jal = 0;
        control->ALUOp = 0b00;
    }
    // sd
    else if (opcode == 0b0100011)
    {
        control->ALUSrc = 1;
        control->MemtoReg = 0;
        control->RegWrite = 0;
        control->MemRead = 1;
        control->MemWrite = 1;
        control->beq = 0;
        control->bne = 0;
        control->blt = 0;
        control->bge = 0;
        control->jal = 0;
        control->ALUOp = 0b00;
    }
    // I-Type
    else if (opcode == 0b0010011)
    {
        control->ALUSrc = 1;
        control->MemtoReg = 0;
        control->RegWrite = 1;
        control->MemRead = 0;
        control->MemWrite = 0;
        control->beq = 0;
        control->bne = 0;
        control->blt = 0;
        control->bge = 0;
        control->jal = 0;
        control->ALUOp = 0b10;
    }

    // jal
    else if (opcode == 0b1101111) // JAL
    {
        control->ALUSrc = 1;
        control->MemtoReg = 0;
        control->RegWrite = 0;
        control->MemRead = 0;
        control->MemWrite = 0;
        control->beq = 0;
        control->bne = 0;
        control->blt = 0;
        control->bge = 0;
        control->jal = 1;
        control->ALUOp = 0b00;
    }

    // B-Type
    else if (opcode == 0b1100011)
    {
        control->ALUSrc = 0;
        control->MemtoReg = 0;
        control->RegWrite = 1;
        control->MemRead = 0;
        control->MemWrite = 0;
        control->ALUOp = 0b01;
        control->jal = 0;
        switch (func3)
        {
        case 0b000:
            control->beq = 1;
            control->bne = 0;
            control->blt = 0;
            control->bge = 0;
            break;
        case 0b001:
            control->beq = 0;
            control->bne = 1;
            control->blt = 0;
            control->bge = 0;
            break;
        case 0b100:
            control->beq = 0;
            control->bne = 0;
            control->blt = 1;
            control->bge = 0;
            break;
        case 0b101:
            control->beq = 0;
            control->bne = 0;
            control->blt = 0;
            control->bge = 1;
            break;
        }
    }
}

int getImm(unsigned instruction)
{

    //I-Type
    unsigned int opcode = instruction & 0b1111111;
    int Imm;
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
