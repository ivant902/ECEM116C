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

class CPU {
private:
	unsigned long PC; //pc 
	 
	

public:
	CPU();
	unsigned long readPC();
	void incPC();
	void setInstructions(vector<unsigned long> instr);
	int numofInstructions;
	vector<unsigned long> instructions;

};

// add other functions and objects here
string hexChartoBinary(char hex);
string hextoBinary(const string &hex);
vector<string> createBinaryFromHex(vector<string> &instructions, int numofInstructions);
vector<string> turnPairsToFullInstruction(vector<string> pairs, vector<string>instructions, int i);