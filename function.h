#include <iostream>
#include <cstdio>
#include "structs.h"
bool add(int rs,int rt,int rd,int reg[]);
void addu(int rs,int rt,int rd,int reg[]);
bool sub(int rs,int rt,int rd,int reg[]);
void And(int rs,int rt,int rd,int reg[]);
void Or(int rs,int rt,int rd,int reg[]);
void Xor(int rs,int rt,int rd,int reg[]);
void Nor(int rs,int rt,int rd,int reg[]);
void Nand(int rs,int rt,int rd,int reg[]);
void slt(int rs,int rt,int rd,int reg[]);
void sll(int rt,int rd,int shamt,int reg[]);
void srl(int rt,int rd,int shamt,int reg[]);
void sra(int rt,int rd,int shamt,int reg[]);
void mult(int rs,int rt,int* HI,int* LO,int reg[],FILE*);
void multu(int rs,int rt,int* HI,int* LO,int reg[],FILE*);
bool addi(int rs,int rt,int immediate,int reg[]);
void addiu(int rs,int rt,int immediate,int reg[]);
bool check_over(int a,int b,int ans);
