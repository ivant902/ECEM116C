#include "CPU.h"

// helper functions 
// immediate generator helpers
unsigned long getItypeImmediate(unsigned long binary)
{
	return((binary>>20) & 4095);
}
unsigned long getItypeShiftAmt(unsigned long binary)
{
	return((binary>>20) & 31);
}
unsigned long getStypeImmHigh(unsigned long binary)
{
	return((binary>>25) & 127);
}
unsigned long getStypeImmLow(unsigned long binary)
{
	return((binary>>7) & 31);
}
unsigned long getStypeImmediate(unsigned long immhigh, unsigned long immlow)
{
	return((immhigh << 5) | immlow);
}
unsigned long getBtypeImmediate(unsigned long binary)
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
unsigned long getUtypeImmediate(unsigned long binary)
{
	return(binary >> 12);
}
unsigned long getJtypeImmediate(unsigned long binary)
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
// register set helpers
unsigned long getRs1(unsigned long binary)
{
	return ((binary >>15) & 31);
}
unsigned long getRs2(unsigned long binary)
{
	return ((binary >>20) & 31);
}
unsigned long getRd(unsigned long binary)
{
	return ((binary >>7) & 31);
}


// CPU member functions
CPU::CPU()
{
	PC = 0; //set PC to 0
	aluMux = false; 
	memWrite = false; 
	regWrite = false;
	instructionType = notype; 
	numofInstructions = 0; 
	operationType = NONE; 
	for (int i = 0; i < 32; i++)
	{
		registers[i] = 0;
	}
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

void CPU::setRegister(unsigned long binary)
{
	rs1 = getRs1(binary); 
	rs2 = getRs2(binary); 
	rd = getRd(binary);
	
}
void CPU::setImmediate(unsigned long binary)
{ 
	switch(instructionType)
		{
			case rtype:
			break;

			case itype:
			immediate = getItypeImmediate(binary);
			shamt = getItypeShiftAmt(binary);
			break;

			case stype:
			immediate = getStypeImmediate(getStypeImmHigh(binary),getStypeImmLow(binary));
			break;

			case btype:
			immediate = getBtypeImmediate(binary);
			break;

			case utype:
			immediate = getUtypeImmediate(binary);
			break;

			case jtype:
			immediate = getJtypeImmediate(binary);
			break;
			
			case notype: 
			exit(1);
		} 
}
// void CPU::generateRegister()
// {
// 	rs1 = registers[rs1];
// 	rs2 = registers[rs2];
// }


void CPU::setControlSignals()
{
	switch(operationType)
	{
		case ADD:
		aluMux = false; 
		regWrite = true; 
		break;
		
		case XOR:
		aluMux = false; 
		regWrite = true; 
		break;

		case SB:
		aluMux = false; 
		regWrite = true; 
		break;

		case SW:
		aluMux = false; 
		regWrite = true; 
		break;

		case BEQ:
		aluMux = false; 
		break; 

		case ORI:
		aluMux = true; 
		memWrite = true; 
		break;

		case SRAI:
		aluMux = true; 
		regWrite = true; 
		break;

		case LB:
		aluMux = true; 
		break;

		case LW:
		aluMux = true; 
		break;

		case LUI:
		aluMux = true; 
		regWrite = true; 
		break;

		case JAL:
		aluMux = true; 
		break;

		case NONE:
		exit(1);
	}	 
}
int32_t CPU::displayReg(int x)
{
	return(registers[x]);
}
unsigned long CPU::ALUMUX()
{
	if (aluMux == 1)
	{return (immediate);}
	else
	{return (rs2);}
}

unsigned long CPU::ALU()
{
	unsigned long answer; 
	unsigned long secondArg = ALUMUX(); 
	//generateRegister();
	switch(operationType)
	{
		case ADD:
		if(regWrite == true){registers[rd] = registers[rs1] + registers[rs2];}
		break;
		
		case XOR:
		if(regWrite == true){registers[rd] = registers[rs1] ^ registers[rs2];}
		break;

		case SB:
		if(regWrite == true){
			unsigned long temp = registers[rs1] + immediate;
			temp = temp >> 31; 
			registers[rs2] = temp; 
		}
		break;

		case SW:
		if(regWrite == true){
			registers[rs2] = registers[rs1] + immediate;
		}
		break;

		case BEQ:
		break; 

		case ORI:
		answer = rs1 | immediate;
		if (memWrite == true){registers[rd] = answer;}
		break;

		case SRAI:
		if (regWrite == true){registers[rd] = registers[rs1] >> shamt;} 
		break;

		case LB:
		break;

		case LW:
		break;

		case LUI:
		if(regWrite == true){
		int32_t temp = immediate;
		registers[rd] = temp << 12;
		}
		break;

		case JAL:
		break;

		case NONE:
		exit(1);
	}
	return answer; 
}


