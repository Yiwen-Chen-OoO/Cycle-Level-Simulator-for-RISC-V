/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-03 09:03:28
 * @LastEditTime: 2019-08-18 16:33:08
 * @LastEditors: Please set LastEditors
 */
#include "Parser.h"
#include <sys/types.h>
#include <unistd.h>
// FIXME, implement this function.
// Here shows an example on how to translate "add x10, x10, x25"
void loadInstructions(Instruction_Memory *i_mem, const char *trace)
{
    printf("Loading trace file: %s\n", trace);

    FILE *fd = fopen(trace, "r");
    if (fd == NULL)
    {
        perror("Cannot open trace file. \n");
        exit(EXIT_FAILURE);
    }

    // Iterate all the assembly instructions
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Addr PC = 0; // program counter points to the zeroth location initially.
    int IMEM_index = 0;
    while ((read = getline(&line, &len, fd)) != -1)
    {
        // Assign program counter
        i_mem->instructions[IMEM_index].addr = PC;

        // Extract operation
        char *raw_instr = strtok(line, " ");
        if (strcmp(raw_instr, "add") == 0 ||
            strcmp(raw_instr, "sub") == 0 ||
            strcmp(raw_instr, "sll") == 0 ||
            strcmp(raw_instr, "srl") == 0 ||
            strcmp(raw_instr, "xor") == 0 ||
            strcmp(raw_instr, "or") == 0 ||
            strcmp(raw_instr, "and") == 0 ||
            strcmp(raw_instr, "slt") == 0 ||
            strcmp(raw_instr, "sltu") == 0 ||
            strcmp(raw_instr, "sra") == 0 ||
            strcmp(raw_instr, "andw") == 0 ||
            strcmp(raw_instr, "subw") == 0 ||
            strcmp(raw_instr, "sllw") == 0 ||
            strcmp(raw_instr, "srlw") == 0 ||
            strcmp(raw_instr, "sraw") == 0)
        {
            parseRType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        else if (strcmp(raw_instr, "ld") == 0 ||
                 strcmp(raw_instr, "addi") == 0 ||
                 strcmp(raw_instr, "slli") == 0 ||
                 strcmp(raw_instr, "slti") == 0 ||
                 strcmp(raw_instr, "sltiu") == 0 ||
                 strcmp(raw_instr, "xori") == 0 ||
                 strcmp(raw_instr, "srli") == 0 ||
                 strcmp(raw_instr, "ori") == 0 ||
                 strcmp(raw_instr, "andi") == 0)
        {
            parseIType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        else if (strcmp(raw_instr, "sb") == 0 ||
                 strcmp(raw_instr, "sh") == 0 ||
                 strcmp(raw_instr, "sw") == 0 ||
                 strcmp(raw_instr, "sd") == 0)
        {
            parseSType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        else if (strcmp(raw_instr, "beq") == 0 ||
                 strcmp(raw_instr, "bne") == 0 ||
                 strcmp(raw_instr, "blt") == 0 ||
                 strcmp(raw_instr, "bge") == 0 ||
                 strcmp(raw_instr, "bltu") == 0 ||
                 strcmp(raw_instr, "bgeu") == 0)
        {
            parseSBType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        else if (strcmp(raw_instr, "jal") == 0)
        {
            parseUJType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }
        // unsigned tmp = i_mem->instructions[IMEM_index].instruction;
        // int cnt = 0;
        // while (cnt < 32)
        // {
        //     printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
        //     ++cnt;
        // }
        // printf("\n");
        IMEM_index++;
        PC += 4;
    }

    fclose(fd);
}

void parseRType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0;

    if (strcmp(opr, "add") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0;
        funct7 = 0;
    }
    else if (strcmp(opr, "sub") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0;
        funct7 = 0b0100000;
    }
    else if (strcmp(opr, "sll") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b001;
        funct7 = 0;
    }
    else if (strcmp(opr, "slt") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b010;
        funct7 = 0;
    }
    else if (strcmp(opr, "sltu") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b11;
        funct7 = 0;
    }
    else if (strcmp(opr, "xor") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b100;
        funct7 = 0;
    }
    else if (strcmp(opr, "srl") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b101;
        funct7 = 0;
    }
    else if (strcmp(opr, "sra") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b101;
        funct7 = 0b0100000;
    }
    else if (strcmp(opr, "or") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b110;
        funct7 = 0;
    }
    else if (strcmp(opr, "and") == 0)
    {
        opcode = 0b0110011;
        funct3 = 0b111;
        funct7 = 0;
    }
    else if (strcmp(opr, "andw") == 0)
    {
        opcode = 0b0111011;
        funct3 = 0;
        funct7 = 0;
    }
    else if (strcmp(opr, "subw") == 0)
    {
        opcode = 0b0111011;
        funct3 = 0;        
        // unsigned tmp = i_mem->instructions[IMEM_index].instruction;
        // int cnt = 0;
        // while (cnt < 32)
        // {
        //     printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
        //     ++cnt;
        // }
        // printf("\n");
        funct7 = 0b0100000;        
        // unsigned tmp = i_mem->instructions[IMEM_index].instruction;
        // int cnt = 0;
        // while (cnt < 32)
        // {
        //     printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
        //     ++cnt;
        // }
        // printf("\n");
    }        // unsigned tmp = i_mem->instructions[IMEM_index].instruction;
        // int cnt = 0;
        // while (cnt < 32)
        // {
        //     printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
        //     ++cnt;
        // }
        // printf("\n");
    else if (strcmp(opr, "sllw") == 0)        
        // unsigned tmp = i_mem->instructions[IMEM_index].instruction;
        // int cnt = 0;
        // while (cnt < 32)
        // {
        //     printf("%d", (tmp & 0x1 << (31 - cnt)) >> (31 - cnt));
        //     ++cnt;
        // }
        // printf("\n");
    {
        opcode = 0b0111011;
        funct3 = 0b001;
        funct7 = 0;
    }
    else if (strcmp(opr, "srlw") == 0)
    {
        opcode = 0b0111011;
        funct3 = 0b101;
        funct7 = 0;
    }
    else if (strcmp(opr, "sraw") == 0)
    {
        opcode = 0b0111011;
        funct3 = 0b101;
        funct7 = 0b0100000;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg) - 1] = '\0';
    unsigned rs_2 = regIndex(reg);

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 + 5));
}

void parseIType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned rd;
    unsigned rs_1;
    int imm;

    if (strcmp(opr, "ld") == 0)
    {
        opcode = 0b0000011;
        funct3 = 0b011;

        char *reg = strtok(NULL, ", ()");
        rd = regIndex(reg);

        reg = strtok(NULL, ", ()");
        imm = atoi(reg);

        reg = strtok(NULL, ", ()");
        rs_1 = regIndex(reg);
    }
    else
    {
        if (strcmp(opr, "addi") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0;
        }
        else if (strcmp(opr, "slli") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b001;
        }
        else if (strcmp(opr, "slti") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b010;
        }
        else if (strcmp(opr, "sltiu") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b011;
        }
        else if (strcmp(opr, "xori") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b100;
        }
        else if (strcmp(opr, "srli") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b101;
        }

        else if (strcmp(opr, "srai") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b101;
            imm = 0b100000;
        }

        else if (strcmp(opr, "ori") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b110;
        }
        else if (strcmp(opr, "andi") == 0)
        {
            opcode = 0b0010011;
            funct3 = 0b111;
        }

        char *reg = strtok(NULL, ", ");
        rd = regIndex(reg);

        reg = strtok(NULL, ", ");
        rs_1 = regIndex(reg);

        reg = strtok(NULL, ", ");
        reg[strlen(reg) - 1] = '\0';
        imm = atoi(reg);
    }

    // printf("rd:  %u\n", rd);
    // printf("rs_1:   %u\n", rs_1);
    // printf("imm:   %u\n", imm);
    // Contruct instruction
    // printf("opcode:  %u \n", opcode);
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (imm << (7 + 3 + 10));
}

void parseSType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode;
    unsigned imm_low;
    unsigned funct3;
    unsigned rs_1;
    unsigned rs_2;
    unsigned imm_high;
    int imm;
    if (strcmp(opr, "sb") == 0)
    {
        opcode = 0b0100011;
        funct3 = 0;
    }
    else if (strcmp(opr, "sh") == 0)
    {
        opcode = 0b0100011;
        funct3 = 0b001;
    }
    else if (strcmp(opr, "sw") == 0)
    {
        opcode = 0b0100011;
        funct3 = 0b010;
    }
    else if (strcmp(opr, "sd") == 0)
    {
        opcode = 0b0100011;
        funct3 = 0b011;
    }

    char *reg = strtok(NULL, ", ()");
    rs_2 = regIndex(reg);

    reg = strtok(NULL, ", ()");
    imm = atoi(reg);

    reg = strtok(NULL, ", ()");
    rs_1 = regIndex(reg);

    imm_low = imm & 0b11111;
    imm_high = (imm >> 5) & 0b1111111;

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (imm_low << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (imm_high << (7 + 5 + 3 + 5 + 5));
}

void parseSBType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode;
    unsigned imm_low;
    unsigned funct3;
    unsigned rs_1;
    unsigned rs_2;
    unsigned imm_high;
    int imm;

    if (strcmp(opr, "beq") == 0)
    {
        opcode = 0b1100011;
        funct3 = 0;
    }
    else if (strcmp(opr, "bne") == 0)
    {
        opcode = 0b1100011;
        funct3 = 0b001;
    }
    else if (strcmp(opr, "blt") == 0)
    {
        opcode = 0b1100011;
        funct3 = 0b100;
    }
    else if (strcmp(opr, "bge") == 0)
    {
        opcode = 0b1100011;
        funct3 = 0b101;
    }
    else if (strcmp(opr, "bltu") == 0)
    {
        opcode = 0b1100011;
        funct3 = 0b110;
    }
    else if (strcmp(opr, "bgeu") == 0)
    {
        opcode = 0b1100011;
        funct3 = 0b111;
    }

    char *reg = strtok(NULL, ", ");
    rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    rs_2 = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg) - 1] = '\0';
    imm = atoi(reg);

    imm_low = (imm & 0b11110) + ((imm >> 11) & 1);
    imm_high = ((imm >> 5) & 0b111111) | ((imm & 0x1000) >> 6);

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (imm_low << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (imm_high << (7 + 5 + 3 + 5 + 5));
}

void parseUJType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode;
    unsigned rd;
    int imm;

    if (strcmp(opr, "jal") == 0)
    {
        opcode = 0b1101111;
    }

    char *reg = strtok(NULL, ", ");
    rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg) - 1] = '\0';
    imm = atoi(reg);

    unsigned imm1 = (imm >> 12) & 0b11111111;
    unsigned imm2 = (imm >> 11) & 1;
    unsigned imm3 = (imm >> 1) & 0b1111111111;
    unsigned imm4 = (imm >> 20) & 1;
    unsigned final_imm = (imm4 << 19) + (imm3 << 9) + (imm2 << 8) + imm1;
    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (final_imm << (7 + 5));
}

int regIndex(char *reg)
{
    unsigned i = 0;
    for (; i < NUM_OF_REGS; i++)
    {
        if (strcmp(REGISTER_NAME[i], reg) == 0)
        {
            break;
        }
    }

    return i;
}
