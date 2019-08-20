#include "Parser.h"

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
    size_t read;

    Addr PC = 0; // program counter points to the zeroth location initially.
    int IMEM_index = 0;
    while ((read = getline(&line, &len, fd)) != -1)
    {
        // Assign program counter
        i_mem->instructions[IMEM_index].addr = PC;
        printf("%s\n", line);
        // Extract operation
        char *raw_instr = strtok(line, " ");

        

        if (strcmp(raw_instr, "add") == 0 ||
            strcmp(raw_instr, "sub") == 0 ||
            strcmp(raw_instr, "sll") == 0 ||
            strcmp(raw_instr, "srl") == 0 ||
            strcmp(raw_instr, "xor") == 0 ||
            strcmp(raw_instr, "or")  == 0 ||
            strcmp(raw_instr, "and") == 0)
        {
            parseRType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
	    } else if (strcmp(raw_instr, "ld") == 0 ||
            strcmp(raw_instr, "addi") == 0 ||
            strcmp(raw_instr, "slli") == 0 ||
            strcmp(raw_instr, "xori") == 0 ||
            strcmp(raw_instr, "srli") == 0 ||
            strcmp(raw_instr, "ori") == 0 ||
            strcmp(raw_instr, "andi") == 0 ||
            strcmp(raw_instr, "jarl") == 0)
        {
            parseIType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        } else if (strcmp(raw_instr, "sd") == 0)
        {
            parseSType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        } else if (strcmp(raw_instr, "beq") == 0 ||
                strcmp(raw_instr, "bne") == 0 ||
                strcmp(raw_instr, "blt") == 0 ||
                strcmp(raw_instr, "bge") == 0)
        {
            parseSBType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);        
        
        }
        else if (strcmp(raw_instr, "jal") == 0)
        {
            parseUJType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

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
        opcode = 51;
        funct3 = 0;
        funct7 = 0;
    } else if (strcmp(opr, "sub") == 0)
    {
        opcode = 51;
        funct3 = 0;
        funct7 = 0x20;
    } else if (strcmp(opr, "sll") == 0)
    {
        opcode = 51;
        funct3 = 0x1;
        funct7 = 0;
    }
    else if (strcmp(opr, "srl") == 0)
    {
        opcode = 51;
        funct3 = 0x5;
        funct7 = 0;
    }
    else if (strcmp(opr, "xor") == 0)
    {
        opcode = 51;
        funct3 = 0x4;
        funct7 = 0;
    }
    else if (strcmp(opr, "or") == 0)
    {
        opcode = 51;
        funct3 = 0x6;
        funct7 = 0;
    }
    else if (strcmp(opr, "and") == 0)
    {
        opcode = 51;
        funct3 = 0x7;
        funct7 = 0;
    }
    
    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    printf("%s\n", reg);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    printf("%s\n", reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg)-1] = '\0';
    unsigned rs_2 = regIndex(reg);
    printf("%s\n", reg);
    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 + 5));
    printf("%u\n", instr->instruction);
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
    //printf("rd_parse%i",rd);
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (imm << (7 + 3 + 10));
    
}

void parseSType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;

    if (strcmp(opr, "sd") == 0)
    {
        opcode = 0x23;
        funct3 = 0x3;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rs_2 = regIndex(reg);
    printf("%s\n", reg);

    reg = strtok(NULL, "(");
    unsigned imm = atoi(reg);
    printf("%s\n", reg);

    reg = strtok(NULL, ")");
    //reg[strlen(reg)-1] = '\0';
    unsigned rs_1 = regIndex(reg);
    printf("%s\n", reg);

    unsigned imm_1 = imm & 31;
    printf("imm_1: %u\n", imm_1);
    unsigned imm_2 = (imm & 4064) >> 5;
    printf("imm_2: %u\n", imm_2);

    instr->instruction |= opcode;
    instr->instruction |= (imm_1 << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (imm_2 << (7 + 5 + 3 + 5 + 5));
    printf("%u\n", instr->instruction);
}

void parseSBType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;

    if (strcmp(opr, "beq") == 0)
    {
        opcode = 0x63;
        funct3 = 0;
    }
    else if (strcmp(opr, "bne") == 0)
    {
        opcode = 0x63;
        funct3 = 0x1;
    }
    else if (strcmp(opr, "blt") == 0)
    {
        opcode = 0x63;
        funct3 = 0x4;
    }
    else if (strcmp(opr, "bge") == 0)
    {
        opcode = 0x63;
        funct3 = 0x5;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);
    printf("%s\n", reg);

    reg = strtok(NULL, ", ");
    unsigned rs_2 = regIndex(reg);
    printf("%s\n", reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg)-1] = '\0';
    unsigned imm = atoi(reg);
    printf("%s\n", reg);

    unsigned imm_1 = (imm & 30) + ((imm & 1024) >> 10);
    unsigned imm_2 = ((imm & 2048) >> 5) + ((imm & 1008) >> 4);

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (imm_1 << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (imm_2 << (7 + 5 + 3 + 5 + 5));
    printf("%u\n", instr->instruction);
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
    for (i; i < NUM_OF_REGS; i++)
    {
        if (strcmp(REGISTER_NAME[i], reg) == 0)
        {
            break;
        }
    }

    return i;
}
