#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include <vector>
using namespace std;


// class instruction { // optional
// public:
// 	bitset<32> instr;//instruction
// 	instruction(bitset<32> fetch); // constructor

// };
enum instructType {notype, rtype, itype, stype, btype, utype, jtype};
enum operation{NONE, ADD, XOR, ORI, SRAI, LB, LW, SB, SW, BEQ, LUI, JAL};
//declaration for immediate gen helper functions 
int32_t getBtypeImmediate(int32_t binary);
int32_t getJtypeImmediate(int32_t binary);

// function declarations that are used in main to bring in the hex bytes in the right order and into binary
string hexChartoBinary(char hex);
string hextoBinary(const string &hex);
vector<string> createBinaryFromHex(vector<string> &instructions, int numofInstructions);
vector<string> turnPairsToFullInstruction(vector<string> pairs, vector<string>instructions, int i);
//CPU class declaration
class CPU {
private:
	unsigned long PC;
public:
	CPU();
	unsigned long getPC(); // basically this is the instruction count
	int32_t readPC(); // read the instruction at the PC
	int32_t getImmediate();
	void incPC();
	// used to bring instructions from main and load them into the CPU
	void setInstructions(vector<int32_t> instr);
	void setOperationAndType(int32_t opcode, int32_t funct3);

	// load the appropriate registers with the values to be used in the alu
	// beware the values in registers are just indexes to which register to acces
	void setRegister(int32_t binary);
	void setImmediate(int32_t binary);
	void setControlSignals();
	void jumpPC();
	int32_t ALUMUX(); // determines to use immediate or rs2 in the alu
void ALU();

	// data variables to store register/immediate/general info
	int32_t registers[33]; 
	uint8_t memory[4096];
	int32_t displayReg(int x);
	int32_t rs1; 
	int32_t rs2;
	int32_t rd; 
	int32_t immediate;
	int32_t shamt; 
	instructType instructionType; 
	operation operationType; 

	// total number of instructions to let the main function know when to stop executing
	int numofInstructions;

	// all instructions in binary
	vector<int32_t> instructions;
	
	// control signals
	bool aluMux; // 0 is rs2, 1 is imm gen
	bool memWrite; 
	bool regWrite;
	bool memRead;

};

