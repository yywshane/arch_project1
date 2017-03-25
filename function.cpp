#include "function.h"
bool add(int rs,int rt,int rd,int reg[]){
	int s=reg[rs];
	int t=reg[rt];
	reg[rd]=reg[rs]+reg[rt];
	return check_over(s,t,reg[rd]);
}
void addu(int rs,int rt,int rd,int reg[]){
	reg[rd]=reg[rs]+reg[rt];
}
bool sub(int rs,int rt,int rd,int reg[]){
	int s=reg[rs];
	int t=reg[rt];
	reg[rd]=reg[rs]-reg[rt];
	return check_over(s,t,reg[rd]);
}
void And(int rs,int rt,int rd,int reg[]){
	reg[rd]=reg[rs]&reg[rt];
}
void Or(int rs,int rt,int rd,int reg[]){
	reg[rd]=reg[rs]|reg[rt];	
}
void Xor(int rs,int rt,int rd,int reg[]){
	reg[rd]=reg[rs]^reg[rt];
}
void Nor(int rs,int rt,int rd,int reg[]){
	reg[rd]=~(reg[rs]|reg[rt]);
}
void Nand(int rs,int rt,int rd,int reg[]){
	reg[rd]=~(reg[rs]&reg[rt]);
}
void slt(int rs,int rt,int rd,int reg[]){
	reg[rd]=reg[rs]<reg[rt];	
}
void sll(int rt,int rd,int shamt,int reg[]){
	reg[rd]=reg[rt]<<shamt;
}
void srl(int rt,int rd,int shamt,int reg[]){
	unsigned int num=reg[rt];
	reg[rd]=num>>shamt;
}
void sra(int rt,int rd,int shamt,int reg[]){
	int num=reg[rt];
	reg[rd]=num>>shamt;	
}
void mult(int rs,int rt,int* HI,int* LO,int reg[],FILE* snap){
	int s,t;
	if(reg[rs]<0&&reg[rt]<0){
		s=-reg[rs];
		t=-reg[rt];
	}
	else{
		s=reg[rs];
		t=reg[rt];
	}
	unsigned long long numRs=(unsigned int)s;
	unsigned long long numRt=(unsigned int)t;
	//std::cout<<reg[rs]<<" "<<reg[rt]<<"\n";
	//std::cout<<numRs<<" "<<numRt<<"\n";
	unsigned long long num=numRs*numRt;
	long hi=num>>32;
	long lo=num<<32>>32;
	//std::cout<<num<<"\n";
	int Hi=hi;
 int Lo=lo;
	if(*HI!=Hi)
		fprintf(snap,"$HI: 0x%08X\n",Hi);
	if(*LO!=Lo)
		fprintf(snap,"$LO: 0x%08X\n",Lo);	
	*HI=hi;
	*LO=lo;	
}
void multu(int rs,int rt,int* HI,int* LO,int reg[],FILE* snap){
	unsigned long long numRs=(unsigned int)reg[rs];
	unsigned long long numRt=(unsigned int)reg[rt];
	//std::cout<<reg[rs]<<" "<<reg[rt]<<"\n";
	//std::cout<<numRs<<" "<<numRt<<"\n";
	unsigned long long num=numRs*numRt;
	long hi=num>>32;
	long lo=num<<32>>32;
	//std::cout<<num<<"\n";
 int Hi=hi;
 int Lo=lo;
	if(*HI!=Hi)
		fprintf(snap,"$HI: 0x%08X\n",Hi);
	if(*LO!=Lo)
		fprintf(snap,"$LO: 0x%08X\n",Lo);	
	*HI=hi;
	*LO=lo;			
}
bool addi(int rs,int rt,int immediate,int reg[]){
	int target=immediate;
	if(target>=32768)
		target-=65536;
	int s=reg[rs];
	reg[rt]=reg[rs]+target;
	return check_over(s,target,reg[rt]);
}
void addiu(int rs,int rt,int immediate,int reg[]){
	int target=immediate;
	if(target>=32768)
		target-=65536;
	reg[rt]=reg[rs]+target;
}


