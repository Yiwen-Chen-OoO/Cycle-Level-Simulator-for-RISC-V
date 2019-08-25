/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-03 09:03:28
 * @LastEditTime: 2019-08-18 16:32:40
 * @LastEditors: Please set LastEditors
 */
#include <stdio.h>
#include <stdlib.h>

#include "Core.h"
#include "Parser.h"

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s %s\n", argv[0], "<trace-file>");

        return 0;
    }

    /* Task One */
    // TODO, (1) parse and translate all the assembly instructions into binary format;
    // (2) store the translated binary instructions into instruction memory.
    Instruction_Memory instr_mem;
    instr_mem.last = NULL;
    loadInstructions(&instr_mem, argv[1]);

    /* Task Two */
    // TODO, implement Core.{h,c}
    Core *core = initCore(&instr_mem);

    /* Task Three - Simulation */
    while (core->tick(core))
        ;

    printf("Simulation is finished.\n");

    free(core);
}
