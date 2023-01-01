#include "CPU.h"

instruction::instruction(bitset<32> fetch)
{
	//cout << fetch << endl;
	instr = fetch;
	//cout << instr << endl;
}

CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	PC += 4;//increment PC
	return instr;
}


bool CPU::Decode(instruction* curr)
{
// cout<<curr->instr<<endl;
return true;
}

unsigned long CPU::readPC()
{
	return PC;
}

void CPU::writePC(unsigned long nextPC) {
	PC = nextPC;
}

int CPU::readMem(int address) {
	return dmemory[address];
}

void CPU::writeMem(int address, bitset<32> data) {
	bitset<32> mask(255);
	int bitData [4];
	bitData[0] = (int)((data & mask).to_ulong());
	bitData[1] = (int)(((data & (mask << 8)).to_ulong()) >> 8);
	bitData[2] = (int)(((data & (mask << 16)).to_ulong()) >> 16);
	bitData[3] = (int)(((data & (mask << 24)).to_ulong()) >> 24);
	for (int i = 0; i < 4; i++) {
		dmemory[address + i] = bitData[i];
	}
}