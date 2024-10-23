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

class CPU {
private:
	unsigned long PC; //pc 

public:
	CPU();
	unsigned long getPC();
	unsigned long readPC();
	void incPC();
	void setInstructions(vector<unsigned long> instr);
	void setOperationAndType(unsigned long opcode, unsigned long funct3);
	void setControlSignals();
	int numofInstructions;
	vector<unsigned long> instructions;
	instructType instructionType; 
	operation operationType; 

};

// add other functions and objects here
string hexChartoBinary(char hex);
string hextoBinary(const string &hex);
vector<string> createBinaryFromHex(vector<string> &instructions, int numofInstructions);
vector<string> turnPairsToFullInstruction(vector<string> pairs, vector<string>instructions, int i);