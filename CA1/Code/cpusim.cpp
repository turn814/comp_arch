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

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[])
{
	/*
	for (int i = 0; i < 32; i++) { 										// debug all
		cout << i << ' ';
	}
	cout << endl;
	*/

	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	bitset<8> instMem[4096];


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile) {
			infile>>line;
			stringstream line2(line);
			int x; 
			line2>>x;
			instMem[i] = bitset<8>(x);
			i++;
		}
	int maxPC= i; 

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU;  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 
	bitset<32> curr;
	instruction instr = instruction(curr);
	bool done = true;

	int regFile [32];
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		//fetch
		curr = myCPU.Fetch(instMem); // fetching the instruction
		instr = instruction(curr);

		// decode
		done = myCPU.Decode(&instr);
		if (done ==false) // break from loop so stats are not mistakenly updated
			break;
		// the rest should be implemented here ...
		// ... 

		bitset<7> opcode;
		for (i = 0; i < 7; i++) {
			opcode[i] = instr.instr[i];
		}

		int opcodeInt = (int)(opcode.to_ulong());
		bitset<5> readReg1;
		bitset<5> readReg2;
		bitset<5> writeReg;
		bitset<3> funct3;
		bitset<7> funct7;
		bitset<12> imm;

		// identify registers
		for (i = 0; i < 5; i++) {
			writeReg[i] = instr.instr[i + 7];
			readReg1[i] = instr.instr[i + 15];
			readReg2[i] = instr.instr[i + 20];
		}

		// funct3 and funct7
		for (i = 0; i < 3; i++) {
			funct3[i] = instr.instr[i + 12];
		}
		for (i = 0; i < 7; i++) {
			funct7[i] = instr.instr[i + 25];
		}
		int funct3Int = (int)(funct3.to_ulong());
		int funct7Int = (int)(funct7.to_ulong());

		// generate immediates
		switch (opcodeInt) {
			case 51: // r-type
			// not necessary?
			break;
			case 19: // i-type
			
			case 3: // load
			
			case 103: // jump
			for (i = 0; i < 12; i++) {
				imm[i] = instr.instr[i + 20];
			}
			break;
			case 35: // store
			for (i = 5; i < 12; i++) {
				imm[i] = instr.instr[i + 25];
			}
			for (i = 0; i < 5; i++) {
				imm[i] = instr.instr[i + 7];
			}
			break;
			case 99: // branch
			for (i = 1; i < 5; i++) {
				imm[i] = instr.instr[i + 7];
			}
			for (i = 5; i < 10; i++) {
				imm[i] = instr.instr[i + 25];
			}
			imm[11] = instr.instr[7];
			imm[12] = instr.instr[31];
			break;
		}
		unsigned long immLong = imm.to_ulong();
		int immIntShift = immLong << 20;
		int immInt = immIntShift >> 20;
		//cout << immInt << " immediate int" << endl;									// debug immGen

		// cout << writeReg.to_ulong() << ' ' << readReg1.to_ulong() << ' '  << readReg2.to_ulong() << ' '  << imm.to_ulong() << endl;
		
		// generate control signals
		bool branch = false;
		bool memRead = false;
		bool memToReg = false;
		bitset<2> aluOp;
		bitset<2> temp1(1);
		bitset<2> temp2(2);
		bool memWrite = false;
		bool aluSrc = false; // false means readReg2 goes to ALU, true means imm goes to ALU
		bool regWrite = false;
		bool jump = false;
		switch (opcodeInt) {
			case 51: // r-type
			regWrite = true;
			switch (funct3Int) {
				case 0:
				// add means aluOp = 00
				if (funct7Int != 0) { // sub
					aluOp = temp1;
				}
				break;
				case 4: // xor
				aluOp = temp2;
				break;
				case 5: // sra
				aluOp = temp2;
			}
			break;
			case 19: // i-type
			aluSrc = true;
			regWrite = true;
			// addi aluOp = 00
			if (funct3Int == 7) { // andi
				aluOp = temp2;
			}
			break;
			case 3: // load
			memRead = true;
			memToReg = true;
			aluSrc = true;
			regWrite = true;
			// aluOp = 00
			break;
			case 103: // jump
			aluSrc = true;
			regWrite = true;
			jump = true;
			// aluOp = 00
			break;
			case 35: // store
			memWrite = true;
			aluSrc = true;
			// aluOp = 00
			break;
			case 99: // branch
			branch = true;
			aluOp = temp1;
			break;
		}

		// Generate RegFile output
		int outReg1 = regFile[(int)(readReg1.to_ulong())];
		int outReg2 = regFile[(int)(readReg2.to_ulong())];

		//** Execute Stage **//
		// ALUSrc Mux
		int aluIn1 = outReg1;
		int aluIn2;
		if (aluSrc) {
			aluIn2 = immInt;
		} else {
			aluIn2 = outReg2;
		}

		// ALU Control and ALU
		int aluOut;
		int aluOpInt = (int)(aluOp.to_ulong());
		switch (aluOpInt) {
			case 0: // add
			aluOut = aluIn1 + aluIn2;
			break;
			case 1: // sub
			aluOut = aluIn1 - aluIn2;
			break;
			case 2: // other
			switch (funct3Int) {
				case 4: // xor
				aluOut = (~aluIn1 & aluIn2) | (aluIn1 & ~aluIn2);
				break;
				case 5: // sra
				aluOut = aluIn1 >> aluIn2;
				break;
				case 7: // and
				aluOut = aluIn1 & aluIn2;
				break;
			}
			break;
		}

		// JALR
		if (jump) {
			//cout << aluOut << " aluOut (jump)" << endl;									// debug jump
			unsigned long jumpPC = aluOut;
			unsigned long nextPC = myCPU.readPC();
			myCPU.writePC(jumpPC);
			regFile[(int)(writeReg.to_ulong())] = nextPC;
			//cout << myCPU.readPC() << " nextPC (jump)" << endl;									// debug jump
			continue;
		}

		// BLT
		if ((aluOut < 0) && branch) {
			//cout << aluOut << " aluOut (branch)" << endl;									// debug branch
			unsigned long branchPC = myCPU.readPC() + immInt - 4;
			myCPU.writePC(branchPC);
			continue;
		}

		//** Memory Stage **//
		int addrIn = aluOut;
		bitset<32> outReg2Bit(outReg2);
		bitset<32> writeDataIn = outReg2Bit;
		int readData;
		if (memRead) {
			int dmem [4];
			for (int i = 0; i < 4; i++) {
				dmem[i] = myCPU.readMem(addrIn + i);
			}
			readData = ((dmem[3] << 24) + (dmem[2] << 16) + (dmem[1] << 8) + (dmem[0]));
			//continue;
		}
		if (memWrite) {
			//cout << outReg2 << " outReg2" << endl;									// debug memWrite
			myCPU.writeMem(addrIn, writeDataIn);
			//cout << writeDataIn << " writeDataIn" << endl;							// debug memWrite
			//cout << addrIn << " addrIn" << endl;									// debug memWrite
			//cout << myCPU.readMem(addrIn + 0) << ' ' << myCPU.readMem(addrIn + 1) << ' ' << myCPU.readMem(addrIn + 2) << ' ' << myCPU.readMem(addrIn + 3) << " dmemory[" << addrIn << ':' << (addrIn + 3) << ']' << endl; // debug memWrite
			// continue;
		}

		//** Writeback Stage **//
		int writeback;
		if (memToReg) {
			writeback = readData;
		} else {
			writeback = aluOut;
		}
		if (regWrite) {
			regFile[(int)(writeReg.to_ulong())] = writeback;
			//cout << writeback << " writeback" << endl; 							// debug writeback
		}
		
		/*
		for (int i = 0; i < 32; i++) {												// debug all
			cout << regFile[i] << ' ';
		}
		cout  << endl;
		*/

		// sanity check
		if (myCPU.readPC() > maxPC)
			break;
	}
	int a0 = regFile[10];
	int a1 = regFile[11];

	/*
	for (int i = 0; i < 32; i++) {											// debug all
		cout << myCPU.readMem(i) << ' ';
	}
	cout << endl;
	*/

	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	  cout << "(" << a0 << "," << a1 << ")" << endl;
	
	return 0;

}