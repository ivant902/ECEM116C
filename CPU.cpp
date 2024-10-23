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
void CPU::setControlSignals()
{
	if (operationType == ADD)
	{
		int j = 0; 
		j++; 
	}
}


// Add other functions here ... 