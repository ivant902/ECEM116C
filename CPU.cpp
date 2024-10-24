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
	    // Extracting individual bits and segments for B-type immediate
    unsigned long imm12 = (binary >> 31) & 1;     // bit 12
    unsigned long imm10_5 = (binary >> 25) & 0x3F; // bits [10:5]
    unsigned long imm4_1 = (binary >> 8) & 0xF;   // bits [4:1]
    unsigned long imm11 = (binary >> 7) & 1;      // bit 11

    // Combine to form the 12-bit immediate
    unsigned long answer = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);

    // Sign-extend the 12-bit immediate to 64-bit (if needed)
    if (answer & (1 << 12)) {
        answer |= 0xFFFFFFFFFFFFF000;  // Set upper bits for sign extension
    }

    return answer;
}
unsigned long getUtypeImmediate(unsigned long binary)
{
	return(binary >> 12);
}
unsigned long getJtypeImmediate(unsigned long binary)
{
	// Extracting individual bits and segments for J-type immediate
    unsigned long imm20 = (binary >> 31) & 1;      // bit 20
    unsigned long imm19_12 = (binary >> 12) & 0xFF; // bits [19:12]
    unsigned long imm11 = (binary >> 20) & 1;      // bit 11
    unsigned long imm10_1 = (binary >> 21) & 0x3FF; // bits [10:1]

    // Combine to form the 20-bit immediate
    unsigned long answer = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);

    // 32-bit sign extension
    if (answer & (1 << 20)) {
        answer |= 0xFFF00000;  // Set upper bits for 32-bit sign extension
    }

    return answer;
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
	memRead = false; 
	instructionType = notype; 
	numofInstructions = 0; 
	operationType = NONE; 
	for (int i = 0; i < 32; i++)
	{
		registers[i] = 0;
	}
	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
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
void CPU::jumpPC()
{
	PC += (immediate/4);
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
			//cout << immediate<< endl; 
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
		memRead = true; 
		break;

		case SW:
		aluMux = false; 
		regWrite = true; 
		memRead = true; 

		break;

		case BEQ:
		aluMux = false; 
		break; 

		case ORI:
		aluMux = true; 
		regWrite = true; 
		break;

		case SRAI:
		aluMux = true; 
		regWrite = true; 
		break;

		case LB:
		aluMux = true; 
		regWrite = true;
		memRead = true;
		break;

		case LW:
		aluMux = true; 
		regWrite = true;
		memRead = true;
		break;

		case LUI:
		aluMux = true; 
		regWrite = true; 
		break;

		case JAL:
		aluMux = true;
		regWrite = true;  
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
				memory[temp]   = registers[rs2] & 0xFF;
            	memory[temp + 1] = (registers[rs2] >> 8) & 0xFF;
            	memory[temp + 2] = (registers[rs2] >> 16) & 0xFF;
            	memory[temp + 3] = (registers[rs2] >> 24) & 0xFF;
			}
		}
		break;

		case BEQ:
		
		break; 

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
            // Store the 32-bit word in the destination register
            registers[rd] = word;
			}
		}
		break;

		case LUI:
		if(regWrite == true){
		registers[rd] = immediate << 12;
		}
		break;

		case JAL:
		break;

		case NONE:
		exit(1);
	}
	return answer; 
}


