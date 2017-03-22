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
	unsigned long num=reg[rt];
	for(int i=0;i<shamt;++i)
		num*=2;
	reg[rd]=num;
}
void srl(int rt,int rd,int shamt,int reg[]){
	unsigned int num=reg[rt];
	for(int i=0;i<shamt;++i)
		num/=2;
	reg[rd]=num;
}
void sra(int rt,int rd,int shamt,int reg[]){
	unsigned int num=reg[rt];
	for(int i=0;i<shamt;++i)
		num/=2;
	reg[rd]=num;
	if(reg[rt]<0){
		reg[rd]-=INT_MIN;
	}	
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
	long hi=num/4294967296;
	long lo=num%4294967296;
	//std::cout<<num<<"\n";
	if(*HI!=hi)
		fprintf(snap,"$HI: 0x%08X\n",hi);
	if(*LO!=lo)
		fprintf(snap,"$LO: 0x%08X\n",lo);	
	*HI=hi;
	*LO=lo;	
}
void multu(int rs,int rt,int* HI,int* LO,int reg[],FILE* snap){
	unsigned long long numRs=(unsigned int)reg[rs];
	unsigned long long numRt=(unsigned int)reg[rt];
	//std::cout<<reg[rs]<<" "<<reg[rt]<<"\n";
	//std::cout<<numRs<<" "<<numRt<<"\n";
	unsigned long long num=numRs*numRt;
	long hi=num/4294967296;
	long lo=num%4294967296;
	//std::cout<<num<<"\n";
	if(*HI!=hi)
		fprintf(snap,"$HI: 0x%08X\n",hi);
	if(*LO!=lo)
		fprintf(snap,"$LO: 0x%08X\n",lo);	
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


