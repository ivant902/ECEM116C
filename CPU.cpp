#include "CPU.h"


CPU::CPU()
{
	PC = 0; //set PC to 0
	instructionType = notype; 
	numofInstructions = 0; 
	operationType = NONE; 
}


unsigned long CPU::getPC()
{
	return PC; 
}
unsigned long CPU::readPC()
{
	return (instructions[PC]);
}
void CPU::incPC()
{
	PC++;
}
void CPU::setInstructions(vector<unsigned long> instr)
{
	instructions = instr; 
}
void CPU::setOperationAndType(unsigned long opcode, unsigned long funct3)
{
	switch(opcode)
		{
			case 51: // R-type instruction (ADD, XOR)
			instructionType = rtype;
			if (funct3 == 0)
			{
				operationType = ADD;
			}
			else if (funct3 == 4)
			{
				operationType = XOR; 
			}
			break;

			case 19: // I-type instruction (ORI, SRAI)
			case 3: // I-type instruction (LB, LW)
			instructionType = itype;
			if (funct3 == 6)
			{
				operationType = ORI;
			}
			else if (funct3 == 5)
			{
				operationType = SRAI; 
			}
			else if (funct3 == 0)
			{
				operationType = LB; 
			}
			else if (funct3 == 2)
			{
				operationType = LW; 
			}
			break;
		
			case 35: // S-type instruction
			instructionType = stype;
			if (funct3 == 0)
			{
				operationType = SB;
			}
			else if (funct3 == 2)
			{
				operationType = SW; 
			}
			break;

			case 99: // B-type instruction
			instructionType = btype;
			operationType = BEQ;
			break;

			case 55: // U-type instruction
			instructionType = utype;
			operationType = LUI;
			break;

			case 111: // J-type instruction 
			instructionType = jtype;
			operationType = JAL;
			break;
		}
}
unsigned long CPU::getRs1(unsigned long binary)
{
	return ((binary >>15) & 31);
}
unsigned long CPU::getRs2(unsigned long binary)
{
	return ((binary >>20) & 31);
}
unsigned long CPU::getRd(unsigned long binary)
{
	return ((binary >>7) & 31);
}
unsigned long CPU::getItypeImmediate(unsigned long binary)
{
	return((binary>>20) & 4095);
}
unsigned long CPU::getItypeShiftAmt(unsigned long binary)
{
	return((binary>>20) & 31);
}
unsigned long CPU::getStypeImmHigh(unsigned long binary)
{
	return((binary>>25) & 127);
}
unsigned long CPU::getStypeImmLow(unsigned long binary)
{
	return((binary>>7) & 31);
}
unsigned long CPU::getStypeImmediate(unsigned long immhigh, unsigned long immlow)
{
	return((immhigh << 5) | immlow);
}
unsigned long CPU::getBtypeImmediate(unsigned long binary)
{
	unsigned long lowest = (binary >> 7) & 1; //1 bit
	unsigned long lower = (binary >> 1) & 15; // 4 bits
	unsigned long higher = (binary >> 17) & 63; // 6 bits
	unsigned long highest = (binary >> 6) & 1; // 1 bit
	unsigned long answer; 
	answer = (highest << 1) | lowest; 
	answer = (answer << 6) | higher; 
	answer = (answer << 4) | lower; 
	return (answer);
}
unsigned long CPU::getUtypeImmediate(unsigned long binary)
{
	return(binary >> 12);
}
unsigned long CPU::getJtypeImmediate(unsigned long binary)
{
	unsigned long lowest = (binary >> 12) & 255;
	unsigned long lower = (binary >> 8) & 1; 
	unsigned long higher = (binary >> 1) & 1023; 
	unsigned long highest = (binary >> 10) & 1;
	unsigned long answer;  
	answer = (highest << 8) | lowest; 
	answer = (answer << 1) | lower; 
	answer = (answer << 10) | higher;  
	return (answer);
}
void CPU::parseInstruction(unsigned long binary)
{ //notype, rtype, itype, stype, btype, utype, jtype}
// NONE, ADD, XOR, ORI, SRAI, LB, LW, SB, SW, BEQ, LUI, JAL
	switch(instructionType)
		{
			case rtype:
			rs1 = getRs1(binary); 
			rs2 = getRs2(binary); 
			rd = getRd(binary);
			break;

			case itype:
			rs1 = getRs1(binary);
			rd = getRd(binary);
			immediate = getItypeImmediate(binary);
			shamt = getItypeShiftAmt(binary);
			break;

			case stype:
			rs1 = getRs1(binary);
			rs2 = getRs2(binary);
			immediate = getStypeImmediate(getStypeImmHigh(binary),getStypeImmLow(binary));
			break;

			case btype:
			rs1 = getRs1(binary);
			rs2 = getRs2(binary);
			immediate = getBtypeImmediate(binary);
			break;

			case utype:
			rd = getRd(binary);
			immediate = getUtypeImmediate(binary);
			break;

			case jtype:
			rd = getRd(binary);
			immediate = getJtypeImmediate(binary);
			break;
			
			case notype: 
			exit(1);
		}
}
void CPU::setControlSignals()
{
	if (operationType == ADD)
	{
		int j = 0; 
		j++; 
	}
}


// Add other functions here ... 