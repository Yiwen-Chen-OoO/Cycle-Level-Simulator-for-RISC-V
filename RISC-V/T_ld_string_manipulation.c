//#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(){
    char line[] = "ld x9, 64(x22)";
    //char line[] = "add x8, x0, x7";
    const char *d = "(), ";
    char *reg;
    reg = strtok(line,d);
    printf("opcode: %s\n",reg);
    int i = 0;
    reg = strtok(NULL,d);

    
    while (reg){
        if (i==0){
          printf("destination: %s\n",reg);
        } else if (i==1){ 
            if (reg[0]!='x'){
            int immi = atoi(reg);
            printf("immi: %d \n",immi);
            } else {
              printf("rs1: %s\n",reg);
            }
        } else {
          printf("rs2: %s\n",reg);
        }
        i++;
        reg = strtok(NULL,d);

    }
    printf("instr %s\n",line);
    char *reg1;
    reg1 = strtok(NULL,d);
    printf("new %s",reg1);
}