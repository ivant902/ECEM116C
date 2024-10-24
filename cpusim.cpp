#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include<fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/
#include <vector>
#include <bitset>

/*
Put/Define any helper function/definitions you need here
*/
string hexChartoBinary(char hex)
{
	switch (toupper(hex)) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return ""; // Invalid hex character
    }
}
string hextoBinary(const string &hex)
{
	string binary; 
	for (int i = 0; i < hex.length(); i++)
	{
		binary += hexChartoBinary(hex[i]);
	}
	return binary; 
}
vector<string> createBinaryFromHex(vector<string> &instructions, int numofInstructions)
{
	vector<string> binaryInstructions; 
	for (int z = 0; z < numofInstructions; z++)
	{
		string binary; 
		binary += hextoBinary(instructions[z]); 
		binaryInstructions.push_back(binary);
	}
	return binaryInstructions; 
}
vector<string> turnPairsToFullInstruction(vector<string> pairs, int i)
{
	vector<string> instructions;
	i--; 
	while(i > 0)
	{
		string instruction; 
		//cout << pairs[i] << endl; 
		instruction += pairs[i];
		i--;
		instruction += pairs[i];
		i--;
		instruction += pairs[i];
		i--;
		instruction += pairs[i];
		i--;
		instructions.insert(instructions.begin(), instruction);
	} 
	return instructions; 
}
vector <unsigned long> stringToInt(vector<string> instr, int numofInstructions)
{
	vector<unsigned long> instructions; 
	for (int w = 0; w < numofInstructions; w++)
	{
		bitset<32> bits(instr[w]);
		instructions.push_back(bits.to_ulong()); 
	}
	return instructions; 
}

int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/
	argc = 2; 
	argv[1] = "24instMem-jswr.txt";
	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}
	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	// reads in file into pairs which is sequential bytes concatenated together
	string line; 
	int i = 0;
	vector<string> pairs; 
	char x,y;
	while (infile) {
			infile>>line;
			stringstream line2(line);
			line2>>x;
			line2>>y; 
			string pair;  
			pair += x; 
			pair += y; 
			pairs.push_back(pair); 
			i++; 
		}
	// i is now the total number of two byte pairs subtracting the endoffile '00'
	i--; 
	// turn the byte pairs into 8 byte strings stored in order in the vector
	vector<string> instructions; 
	instructions = turnPairsToFullInstruction(pairs, i);

	// use 'i' to define the total number of instructions from file
	int numofInstructions = i/4; 
	int maxPC = numofInstructions; 

	// turn string hex instructions into string binary
	instructions = createBinaryFromHex(instructions, numofInstructions); 

	//turn string binary to int

	vector<unsigned long> binaryInstructions;
	binaryInstructions = stringToInt(instructions, numofInstructions);

	// at this point the instructions vector holds all instructions in binary
	// numofInstructions is the total count of instructions to be ran
	
	

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU;  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 
	myCPU.setInstructions(binaryInstructions);
	myCPU.numofInstructions = numofInstructions; 
	
	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		//fetch
		unsigned long binary = myCPU.readPC();
		
		// decode
		unsigned long opcode = binary & 127;
		unsigned long funct3 = binary >> 12 & 7; 
		myCPU.setOperationAndType(opcode, funct3);
		myCPU.setRegister(binary);
		myCPU.setImmediate(binary);
		myCPU.setControlSignals();
		if ((myCPU.operationType == BEQ) && (myCPU.registers[myCPU.rs1] == myCPU.registers[myCPU.rs2]))
		{
			myCPU.jumpPC();
			continue;
		}
		else if (myCPU.operationType == JAL)
		{ 
			myCPU.jumpPC();
			continue;
		}
		myCPU.ALU();
		
		// ... 
		myCPU.incPC();
		if (myCPU.getPC() >= maxPC)
			break;
	}
	for (int z = 1; z < 33; z++)
	{
		cout << myCPU.displayReg(z) << endl;
	}
	int32_t a0 = myCPU.displayReg(10);
	int32_t a1 = myCPU.displayReg(11);  
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	cout << "(" << a0 << "," << a1 << ")" << endl;

	return 0;

}