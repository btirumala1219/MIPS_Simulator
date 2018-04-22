//Barath Tirumala
//4/19/2018
//CDA Project

#include "spimcore.h"
#include <stdio.h>


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    int control = (int)ALUControl;

    switch(control){
      case 0: //control is 000, so Z = A+B
          *ALUresult = A + B;
          break;
      case 1: //control is 001, so Z = A-B
          *ALUresult = A - B;
          break;
      case 2: //control is 010, if A is less than B then Z is 1, otherwise 0
          if((signed)A <(signed)B)
              *ALUresult = 1;
          else
              *ALUresult = 0;
          break;
      case 3: //control is 011, same as above however these are unsigned
          if(A < B)
              *ALUresult = 1;
          else
              *ALUresult = 0;
          break;
      case 4: //control is 100, A AND B
          *ALUresult = A & B;
          break;
      case 5: //control is 101, A OR B
          *ALUresult = A | B;
          break;
      case 6: //control is 110, shift left 16 bits
          *ALUresult = B << 16;
          break;
      case 7: //control is 111, Not A
          *ALUresult = ~A;
          break;
    }

    //If result is 0, then set zero value to 1, otherise 0
    if(*ALUresult == 0){
        *Zero = 1;
    }
    else{
        *Zero = 0;
    }
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    unsigned address = PC>>2; //we need to multiply by 4 to get exact address needed
    if(PC % 4 !=0){
      return 1; //if not divisible by 4 then return 1
    }

    *instruction = Mem[address]; //otherwise set instruction at proper address
    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // we only want the bytes relevant for the respective instruction. so shift until then

    *op = (instruction >> 26) & 0b00000000000000000000000000111111; //31-26

    *r1 = (instruction >> 21) & 0b11111; // 25-21

    *r2 = (instruction >> 16) & 0b11111; //20-16

    *r3 = (instruction >> 11) & 0b11111; //15-11

    *funct = instruction & 0b00000000000000000000000000111111; //5-0

    *offset = instruction & 0b00000000000000001111111111111111; //15-0

    *jsec = instruction & 0b00000011111111111111111111111111; //25-0

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // assign values based on which op code is given. Assign the respective values as labeled in spimcore.h

    switch(op){
        case 0: //r-type
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 7;
            break;
        case 8: //addi
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
        case 10: //slt
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 2;
            break;
        case 11: //sltu
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0 ;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 3;
            break;
        case 4: //beq
            controls->RegDst = 2;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->ALUOp = 1;
            break;
        case 35: //lw
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->MemtoReg = 1;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
        case 15: //lui
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 6;
            break;
        case 43: //sw
            controls->RegDst = 2; // 2 for don't care
            controls->RegWrite = 0;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->MemtoReg = 2;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
        case 2: //j
            controls->RegDst = 0;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 1;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
        default:
            return 1;

    }
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    //all that is needed is fill data with the register values specified
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    unsigned msb = offset >> 15; //get most sig bit
    if(msb == 1){ //i msb is 1 then pad on
        *extended_value = offset | 0xFFFF0000;
    }
    else{ //otherwise0
        *extended_value = offset & 0x0000FFFF;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //data is determined based on alusrc
    if (ALUSrc == 1) {
      data2 = extended_value;
    }

    if (ALUOp == 7) {
      switch(funct) {
        case 4:
          ALUOp = 6;
          break;
        case 32: //add
          ALUOp = 0;
          break;
        case 34: //subtract
          ALUOp = 1;
          break;
        case 36: //and
          ALUOp = 4;
          break;
        case 37: //or
          ALUOp = 5;
          break;
        case 39: //nor
          ALUOp = 7;
          break;
        case 42: //slt
          ALUOp = 2;
          break;
        case 43: //sltu
          ALUOp = 3;
          break;

        default: //IF HALT RETURN 1
          return 1;
      }
      // call the ALU function
      ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
    else {
      // for all nonfunctions
      ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

    //If we are reading we need teh address first, multiply by 4 to get it assuming its not multiple of 4
    if(MemRead == 1){
        if((ALUresult%4) == 0){
            *memdata = Mem[ALUresult >> 2];
        }
        else{
            return 1;
        }
    }

    //if we are writing, write to 4 times address memory
    if(MemWrite == 1){
        if((ALUresult%4) == 0){
            Mem[ALUresult>>2] = data2;
        }
        else{
            return 1;
        }
    }
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // check if set to actually write
    if(RegWrite==1){
        //Memory to register
        if(MemtoReg == 1 && RegDst == 0){
            Reg[r2] = memdata;
        }
        //memory to register but to r3
        else if(MemtoReg == 1 && RegDst == 1){
            Reg[r3] = memdata;
        }
        //store ALUResult in r2
        else if(MemtoReg == 0 && RegDst == 0){
            Reg[r2] = ALUresult;
        }
        //store ALUResult in r3
        else if(MemtoReg == 0 && RegDst == 1){
            Reg[r3] = ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4; //update by 4 each time
    if(Jump == 1){
        *PC = (jsec << 2) | (*PC & 0xf0000000);
    }
    if(Zero == 1 && Branch == 1){
        *PC += extended_value << 2;
    }

}
