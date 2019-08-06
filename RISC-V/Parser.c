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
	    }

        if (strcmp(raw_instr, "ld") == 0 ||
            strcmp(raw_instr, "addi") == 0 ||
            strcmp(raw_instr, "slli") == 0 ||
            strcmp(raw_instr, "xori") == 0 ||
            strcmp(raw_instr, "srli") == 0 ||
            strcmp(raw_instr, "ori") == 0 ||
            strcmp(raw_instr, "andi") == 0 ||
            strcmp(raw_instr, "jarl") == 0)
        {
            parseItype(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        IMEM_index++;
        PC += 4;
    }

    fclose(fd);
}

void parseItype(char *opr, Instruction *instr)
{
    // I-type：ld, addi, slli, xori, srli, ori, andi, jarl;
    // ld, sd
    
    if (strcmp(opr, "ld") == 0)
    {
        unsigned opcode = 0b0000011;
        unsigned funct3 = 0b011;
        unsigned funct7 = 0;

        const char *d = " ,()";
        char *reg = strtok(NULL, d);
        unsigned rd = regIndex(reg);
        
        // reg below is actually immi
        reg = strtok(NULL, d);
        int immi = atoi(reg);

        reg = strtok(NULL, d);
        unsigned rs_1 = regIndex(reg);
    } else {

        if (strcmp(opr, "addi") == 0)
        {
            unsigned opcode = 0b0010011;
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } else if (strcmp(opr, "slli") == 0)
        {
            unsigned opcode = 0b0010011;        //Not yet modified from here
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } else if (strcmp(opr, "xori") == 0)
        {
            unsigned opcode = 0b0010011;
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } else if (strcmp(opr, "srli") == 0)
        {
            unsigned opcode = 0b0010011;
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } else if (strcmp(opr, "ori") == 0)
        {
            unsigned opcode = 0b0010011;
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } else if (strcmp(opr, "andi") == 0)
        {
            unsigned opcode = 0b0010011;
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } else if (strcmp(opr, "jarl") == 0)
        {
            unsigned opcode = 0b0010011;
            unsigned funct3 = 0;
            unsigned funct7 = 0;
        } 
    }
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
    } 
    
    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg)-1] = '\0';
    unsigned rs_2 = regIndex(reg);

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 + 5));
  
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
