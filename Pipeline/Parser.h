/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-03 09:03:28
 * @LastEditTime: 2019-08-14 13:16:51
 * @LastEditors: Please set LastEditors
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Instruction_Memory.h"
#include "Registers.h"

void loadInstructions(Instruction_Memory *i_mem, const char *trace);
void parseRType(char *opr, Instruction *instr);
void parseIType(char *opr, Instruction *instr);
void parseSType(char *opr, Instruction *instr);
void parseSBType(char *opr, Instruction *instr);
void parseUJType(char *opr, Instruction *instr);
int regIndex(char *reg);
