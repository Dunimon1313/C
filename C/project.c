// Lester Miller and Zachary Taylor
// CDA 3103
// MIPS Group Project
// Professor Sarah Angell
// Due: 4/19/20
// This program simulates a simplified MIPS processor


#include "spimcore.h"
#include <stdio.h>

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	switch ((int)ALUControl) {

	//addition
	case 0:
		*ALUresult = A + B;
		break;

	//subtraction
	case 1:
		*ALUresult = A - B;
		break;
	//less than
	case 2:
		if ((signed)A < (signed)B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
		break;
	//unsigned less than
	case 3:
		if (A < B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
		break;
	//and--bitwise
	case 4:
		*ALUresult = A & B;
		break;
	//or--bitwise
	case 5:
		*ALUresult = A | B;
		break;
	//A<0
	case 6:
		*ALUresult = B << 16;
		break;
	//not--bitwise
	case 7:
		*ALUresult = ~A;
		break;
	}
	//set Zero = 1 iff aluresult=0
	if (*ALUresult == 0) {
		*Zero = 1;
	}
	else
		*Zero = 0;
	}


/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	// sends halt condition if PC isn't properly aligned
	if (PC % 4 != 0)
		return 1;
	
	// multiplies PC by four to obtain correct address
	unsigned index = PC >> 2;
	
	// set intruction equal to memory at proper address
	*instruction = Mem[index];
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	//break input into: op,r1,r2,r3,funct,offset,jsec

	//op-- instructions 31 to 26
	*op = (instruction >> 26) & 0b00000000000000000000000000111111;
	
	//r1-- instructions 25 to 21
	*r1 = (instruction >> 21) & 0b11111;
	
	//r2-- instructions 20 to 16
	*r2 = (instruction >> 16) & 0b11111;
	
	//r3-- instructions 15 to 11
	*r3 = (instruction >> 11) & 0b11111;
	
	//funct-- instructions 5 to 0
	*funct = instruction & 0b00000000000000000000000000111111;
	
	//offset-- instructions 15 to 0
	*offset = instruction & 0b00000000000000001111111111111111;
	
	//jsec-- instructions 25 to 0
	* jsec = instruction & 0b00000011111111111111111111111111;
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
	// initialize controls
	controls->RegDst = 0;
	controls->Jump = 0;
	controls->Branch = 0;
	controls->MemRead = 0;
	controls->MemtoReg = 0;
	controls->ALUOp = 0;
	controls->MemWrite = 0;
	controls->ALUSrc = 0;
	controls->RegWrite = 0;
	
	switch (op) {
		
		//register type: aluop = 0111 = 7(r-type), regdst = 1 since register is being wrote, the alusrc = 0 since 2nd op of alu is from a register
		case 0:
			controls->RegDst = 1;
			controls->ALUOp = 7;
			controls->RegWrite = 1;
			break;
			
		//load upper immediate:
		case 15:
			controls->ALUOp = 6;
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			break;
			
		//LW: memread is active since data has to be read in order to be loaded, memtoreg is active so the instruction may proceed,
			//alusrc=1 since load is immediate type, regwrite is active since a register is being written, aluop = 0
		case 35:
			controls->MemRead = 1;
			controls->MemtoReg = 1;
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			break;
			
		//SW: memwrite is being expressed through writing, alusrc = 1( given from instruction ), aluop=0
		case 43:
			controls->RegDst = 2;
			controls->MemtoReg = 2;
			controls->MemWrite = 1;
			controls->ALUSrc = 1;
			break;
			
		//set<immediate: alusrc is 1 because it is an immediate instruction, the ALUop is 0010 = 2 meaning the alu will do set less than,
			//regwrite is on because a register is being written
		case 11:
			controls->RegDst = 1;
			controls->ALUOp = 3;
			controls->RegWrite = 1;
			break;
			
		//branch eq: branch=1 b/c the insrtuction is branch type and = is used, alusrc=1 b/c it is present in second operand of instruction
		case 4:
			controls->RegDst = 2;
			controls->Branch = 1;
			controls->MemtoReg = 2;
			controls->ALUOp = 1;
			break;
			
		//add immediate
		case 8:
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			break;
			
		//set<
		case 10:
			controls->RegDst = 1;
			controls->ALUOp = 2;
			controls->RegWrite = 1;
			break;
			
		//jump: aluop=0 which is nop or add, the or is b/c the lower 26 bits are shifted to the left twice and added with 4 bits from PC, alusrc=1 b/c it
			//is present in second operand of instruction
		case 2:
			controls->Jump = 1;
			break;
			
		default:
			//out of project scope
			return 1;
	}
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	// read registers r1 and r2 from reg and write, respectively, read values to data1 and data2. 
	// using an array benefits the alu's usage of these values
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	// takes offset and checks if 16th bit=1, if yes, then it is negative and sign will be extended, if 0, then ignored
	unsigned negtest = offset >> 15;
	
	//if MSB = 1, then #=-, extend with 1s
	if (negtest == 1)
		*extended_value = offset | 0xffff0000;
	
	//oextend with 0
	else
		*extended_value = offset & 0x0000ffff;
}


/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	if (ALUSrc == 1) {
		data2 = extended_value;
	}
		//r-type: checks if alusrc=7, if yes, then r-type
	if (ALUOp == 7) {
		switch (funct) {
			
		//unsigned addition
		case 32:
			ALUOp = 0;
			break;
			
		//unsigned subtraction
		case 34:
			ALUOp = 1;
			break;
			
		//set<signed
		case 42:
			ALUOp = 2;
			break;
			
		//set<unsigned
		case 43:
			ALUOp = 3;
			break;
			
		//and
		case 36:
			ALUOp = 4;
			break;
			
		//or
		case 37:
			ALUOp = 5;
			break;
			
		//shift right variable left
		case 4:
			ALUOp = 6;
			break;
			
		//nor
		case 39:
			ALUOp = 7;
			break;
			
		//not known
		default: 
			return 1;
		}
	// call ALU function
		ALU(data1, data2, ALUOp, ALUresult, Zero);
	}
	else{
		//call alu for non functions
		ALU(data1, data2, ALUOp, ALUresult, Zero);
	}
	return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	// reading
	if (MemRead == 1) {
		if ((ALUresult % 4) == 0) {
			*memdata = Mem[ALUresult >> 2];
		}
		else
			return 1;
	}

	// writting
	if (MemWrite == 1) {
		if ((ALUresult % 4) == 0) {
			Mem[ALUresult >> 2] = data2;
		}
		else
			return 1;
	}
	return 0;
}


/* Write Register */
/* 10 Points */											
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if (RegWrite == 1) {
		if (MemtoReg == 1) {
			if (RegDst == 1)
				Reg[r3] = memdata; //Write memdata to rd 
			else
				Reg[r2] = memdata; // Write memdata to rt
		}
		else {
			if (RegDst == 1)
				Reg[r3] = ALUresult; // Write alu results to rd
			else
				Reg[r2] = ALUresult; // Write alu results to rt
		}
	}
}


/* PC update */
/* 10 Points */		
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //PC is incremented by 4 after each instruction
    *PC += 4;
    
    //if branch=1, pc will = sign extended value will shift left(2)
    if(Zero == 1 && Branch == 1)
        *PC += extended_value << 2;
        
    //if (Jump == 1)
    if(Jump == 1)
        *PC = (jsec << 2) | (*PC & 0xf0000000);
} 

