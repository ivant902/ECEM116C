#include "CPU.h"

// helper functions 
// immediate generator helpers
unsigned long getBtypeImmediate(unsigned long binary)
{
    unsigned long imm12 = (binary >> 31) & 1;     
    unsigned long imm10_5 = (binary >> 25) & 0x3F;
    unsigned long imm4_1 = (binary >> 8) & 0xF; 
    unsigned long imm11 = (binary >> 7) & 1; 
    unsigned long answer = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    if (answer & (1 << 12)) {answer |= 0xFFFFFFFFFFFFF000;}
    return answer;
}
unsigned long getJtypeImmediate(unsigned long binary)
{
    unsigned long imm20 = (binary >> 31) & 1;
    unsigned long imm19_12 = (binary >> 12) & 0xFF;
    unsigned long imm11 = (binary >> 20) & 1;
    unsigned long imm10_1 = (binary >> 21) & 0x3FF;
    unsigned long answer = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
    if (answer & (1 << 20)) {answer |= 0xFFF00000;}
    return answer;
}
// CPU member functions
CPU::CPU() : PC(0), numofInstructions(0), aluMux(false), memWrite(false), regWrite(false), memRead(false), instructionType(notype), operationType(NONE)
{ 
	for (int i = 0; i < 32; i++){registers[i] = 0;}
	for (int i = 0; i < 4096; i++){memory[i] = 0;}
}
unsigned long CPU::getPC(){return PC;}
unsigned long CPU::readPC(){return (instructions[PC/4]);}
unsigned long CPU::getImmediate(){return immediate;}
void CPU::setInstructions(vector<unsigned long> instr){instructions = instr;}
int32_t CPU::displayReg(int x) {return(registers[x]);}
void CPU::incPC()
{
	PC = PC+4;
}
void CPU::jumpPC()
{
	PC += immediate; 
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
	rs1 = (binary >>15) & 31; 
	rs2 = (binary >>20) & 31; 
	rd = (binary >>7) & 31;
	
}
void CPU::setImmediate(unsigned long binary)
{ 
	switch(instructionType)
		{
			case rtype:
			break;

			case itype:
			immediate = (binary>>20) & 4095;
			shamt = (binary>>20) & 31;
			break;

			case stype:
			immediate = (((binary>>25) & 127) << 5) | ((binary>>7) & 31);
			break;

			case btype:
			immediate = getBtypeImmediate(binary);
			break;

			case utype:
			immediate = binary >> 12;
			break;

			case jtype:
			immediate = getJtypeImmediate(binary);
			break;
			
			case notype: 
			exit(1);
		} 
}
void CPU::setControlSignals()
{
	switch(operationType)
	{
		case ADD: regWrite = true; break;
		case XOR: regWrite = true; break;
		case SB: regWrite = true; memRead = true; break;
		case SW: regWrite = true; memRead = true; break;
		case BEQ: break; 
		case ORI: aluMux = true; regWrite = true; break;
		case SRAI: aluMux = true; regWrite = true; break;
		case LB: aluMux = true; regWrite = true; memRead = true; break;
		case LW: aluMux = true; regWrite = true; memRead = true; break;
		case LUI: aluMux = true; regWrite = true; break;
		case JAL: aluMux = true; regWrite = true; break; 
		case NONE: exit(1);
	}	 
}
unsigned long CPU::ALUMUX()
{
	if (aluMux == 1)
	{return (immediate);}
	else
	{return (rs2);}
}
void CPU::ALU()
{ 
	switch(operationType)
	{
		case ADD:
		if(regWrite == true){registers[rd] = registers[rs1] + registers[rs2];}
		break;
		
		case XOR:
		if(regWrite == true){registers[rd] = registers[rs1] ^ registers[rs2];}
		break;

		case SB:
		if(regWrite == true && memRead == true){
			unsigned long temp = registers[rs1] + immediate;
			if (temp < 4096)
			{
				memory[temp] = registers[rs2] & 0xFF;
			}
		}
		break;

		case SW:
		if(regWrite == true && memRead == true){
			unsigned long temp = registers[rs1] + immediate;
			if (temp < 4096 )
			{
				memory[temp] = registers[rs2] & 0xFF;
            	memory[temp + 1] = (registers[rs2] >> 8) & 0xFF;
            	memory[temp + 2] = (registers[rs2] >> 16) & 0xFF;
            	memory[temp + 3] = (registers[rs2] >> 24) & 0xFF;
			}
		}
		break;

		case BEQ: break; 

		case ORI:
		if (regWrite == true){registers[rd] = registers[rs1] | immediate;}
		break;

		case SRAI:
		if (regWrite == true){registers[rd] = registers[rs1] >> shamt;} 
		break;

		case LB:
		if (regWrite == true && memRead == true){
			unsigned long temp = registers[rs1] + immediate;
			if (temp < 4096)
			{
				int8_t byte =  memory[temp];
				registers[rd] = (int32_t) byte; 
			}
		}
		break;

		case LW:
		if (regWrite == true && memRead == true){
			unsigned long temp = registers[rs1] + immediate; 
			if (temp < 4096)
			{
				int32_t word = memory[temp] |
                           (memory[temp + 1] << 8) |
                           (memory[temp + 2] << 16) |
                           (memory[temp + 3] << 24);
            registers[rd] = word;
			}
		}
		break;

		case LUI:
		if(regWrite == true){
		registers[rd] = immediate << 12;
		}
		break;

		case JAL: break;

		case NONE:
		exit(1);
	} 
}


