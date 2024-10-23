#include "CPU.h"


CPU::CPU()
{
	PC = 0; //set PC to 0
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


// Add other functions here ... 