#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include "function.h"
using namespace std;

int main()
{	
	//xxd -c4 -b filename :show binary file in linux
	ifstream input( "iimage.bin", ifstream::binary );
	if(input.fail()){
		input.close();
		input.open( "../testcase/iimage.bin", ifstream::binary );
	}
	ifstream dInput( "dimage.bin", ifstream::binary );
	if(dInput.fail()){
		dInput.close();
		dInput.open( "../testcase/dimage.bin", ifstream::binary );
	}
    vector<int> iBuffer;
    vector<word> instruction;
    vector<int> dBuffer;
    int memory[1024];
    int reg[32]={0};
    int PC,inum=0,dnum=0;
    int HI=0,LO=0;
    FILE *snap;
    FILE *error;
    snap=fopen("snapshot.rpt","w");
    error=fopen("error_dump.rpt","w");
    
	char in;
	input.get(in);
    while(!input.eof()){
    	iBuffer.push_back((int)in);
		input.get(in);
	}
	input.close();
	
	for(int i=0;i<iBuffer.size();++i){
		if(iBuffer[i]<0)
			iBuffer[i]+=256;
	}
	
	PC=(iBuffer[0]<<24)+(iBuffer[1]<<16)+(iBuffer[2]<<8)+iBuffer[3];
    inum=(iBuffer[4]<<24)+(iBuffer[5]<<16)+(iBuffer[6]<<8)+iBuffer[7];
    word trans;
    for(int i=8;i<iBuffer.size();i+=4){
    	trans.op=iBuffer[i]/4;
		trans.rs=(iBuffer[i]%4)*8+iBuffer[i+1]/32;
		trans.rt=iBuffer[i+1]%32;
		trans.rd=iBuffer[i+2]/8;
		trans.shamt=(iBuffer[i+2]%8)*4+iBuffer[i+3]/64;
		trans.funct=iBuffer[i+3]%64;
		trans.immediate=iBuffer[i+2]*256+iBuffer[i+3];
		trans.address=trans.immediate+(trans.rt<<16)+(trans.rs<<21);
		instruction.push_back(trans);
	}
	
	/*
	for(int i=0;i<instruction.size();++i)
        cout<<i<<" :"<<instruction[i].op<<" "<<instruction[i].rs<<" "<<instruction[i].rt<<" "<<instruction[i].rd<<" "<<instruction[i].shamt<<" "<<instruction[i].funct<<" "<<instruction[i].immediate<<" "<<instruction[i].address<<" "<<"\n";
	*/
	
	
	dInput.get(in);
    while(!dInput.eof()){
    	dBuffer.push_back((int)in);
		dInput.get(in);
	}
	dInput.close();
	for(int i=0;i<dBuffer.size();++i){
		if(dBuffer[i]<0)
			dBuffer[i]+=256;
	}
	
	reg[29]=(dBuffer[0]<<24)+(dBuffer[1]<<16)+(dBuffer[2]<<8)+dBuffer[3];
    dnum=(dBuffer[4]<<24)+(dBuffer[5]<<16)+(dBuffer[6]<<8)+dBuffer[7];
    for(int i=8,j=0;i<dBuffer.size();++i,++j){
    	memory[j]=dBuffer[i];
	}
	
	/*
	for(int i=0;i<dBuffer.size()-8;++i)
		cout<<memory[i]<<"\n";
	*/
	/*
	reg[0]=INT_MAX;
	reg[1]=0;
	sll(0,1,1,reg);
	cout<<reg[1];
	*/
	
	fprintf(snap,"cycle 0\n");	
	for(int i=0;i<32;++i)
		fprintf(snap,"$%02d: 0x%08X\n",i,reg[i]);
	fprintf(snap,"$HI: 0x%08X\n",HI);
	fprintf(snap,"$LO: 0x%08X\n",LO);
	fprintf(snap,"PC: 0x%08X\n\n\n",PC);	
	
	int init=PC;
	int end=init+(inum-1)*4;
	int i=(PC-init)/4;
	int cycle=0;
	bool overwrite_HI_LO=false;
	bool mis_ali=false;
	bool addr_over=false;
	bool num_over;
	while(instruction[i].op!=63 && !mis_ali && !addr_over){
    	PC+=4;
		fprintf(snap,"cycle %d\n",++cycle);
		//cout<<"cycle "<<cycle<<" i= "<<i<<"\n";
		switch(instruction[i].op){
			//R_type
			case 0:		
				switch(instruction[i].funct){
					case 32:	//add
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							num_over=add(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);
							if(num_over)
								fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 33:	//addu
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							addu(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 34:	//sub
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							num_over=sub(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);
							if(num_over)
								fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 36:	//and
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							And(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 37:	//or
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							Or(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 38:	//xor
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							Xor(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 39:	//nor
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							Nor(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 40:	//nand
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							Nand(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 42:	//slt
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							slt(instruction[i].rs,instruction[i].rt,instruction[i].rd,reg);
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 0:		//sll
						if(instruction[i].shamt!=0){
							if(instruction[i].rd!=0){
								int num=reg[instruction[i].rd];
								sll(instruction[i].rt,instruction[i].rd,instruction[i].shamt,reg);
								if(reg[instruction[i].rd]!=num)
									fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
							}
							else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						}
						break;
					case 2:		//srl
						if(instruction[i].rd!=0){
							if(instruction[i].shamt!=0){
								int num=reg[instruction[i].rd];
								srl(instruction[i].rt,instruction[i].rd,instruction[i].shamt,reg);
								if(reg[instruction[i].rd]!=num)
									fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
							}
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 3:		//sra
						if(instruction[i].rd!=0){
							if(instruction[i].shamt!=0){
								int num=reg[instruction[i].rd];
								sra(instruction[i].rt,instruction[i].rd,instruction[i].shamt,reg);
								if(reg[instruction[i].rd]!=num)
									fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);
							}
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						break;
					case 8:		//jr
						PC=reg[instruction[i].rs];
						break;
					case 24:	//mult
						if(overwrite_HI_LO)
							fprintf(error,"In cycle %d: Overwrite HI-LO registers\n",cycle);
						mult(instruction[i].rs,instruction[i].rt,&HI,&LO,reg,snap);
						overwrite_HI_LO=true;
						break;
					case 25:	//multu
						if(overwrite_HI_LO)
							fprintf(error,"In cycle %d: Overwrite HI-LO registers\n",cycle);
						multu(instruction[i].rs,instruction[i].rt,&HI,&LO,reg,snap);
						overwrite_HI_LO=true;
						break;
					case 16:	//mfhi
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							reg[instruction[i].rd]=HI;
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						overwrite_HI_LO=false;
						break;
					case 18:	//mflo
						if(instruction[i].rd!=0){
							int num=reg[instruction[i].rd];
							reg[instruction[i].rd]=LO;
							if(reg[instruction[i].rd]!=num)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rd,reg[instruction[i].rd]);	
						}
						else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
						overwrite_HI_LO=false;
						break;
					default:
						//cout<<"error\n";
						break;
				}
				break;
			//I_type
			case 8:		//addi
				if(instruction[i].rt!=0){
					int num=reg[instruction[i].rt];
					num_over=addi(instruction[i].rs,instruction[i].rt,instruction[i].immediate,reg);
					if(reg[instruction[i].rt]!=num)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
					if(num_over)
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 9:		//addiu
				if(instruction[i].rt!=0){
					int num=reg[instruction[i].rt];
					addiu(instruction[i].rs,instruction[i].rt,instruction[i].immediate,reg);
					if(reg[instruction[i].rt]!=num)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);	
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 35:	//lw
				if(instruction[i].rt!=0){
					//cout<<"rs: "<<reg[instruction[i].rs]<<", rt: "<<reg[instruction[i].rt]<<", immediate: "<<instruction[i].immediate<<"\n";
					int j=reg[instruction[i].rt];
					int num=instruction[i].immediate;
					if(num>=32768)
						num-=65536;
					int target=num+reg[instruction[i].rs];
					if(check_over(num,reg[instruction[i].rs],target+3))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
					else{
						if(check_over(num,reg[instruction[i].rs],target))
							fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
					}
					if(target+3<1024 && target>=0){
						if(target%4==0){
							unsigned int num=(memory[target]<<24)+(memory[target+1]<<16)+(memory[target+2]<<8)+memory[target+3];
							reg[instruction[i].rt]=num;
							if(reg[instruction[i].rt]!=j)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
						}
						else{
							fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
							mis_ali=true;
						}
					}
					else{
						fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
						addr_over=true;
						if(target%4!=0)
							fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
					}		
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 33:	//lh
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					int num=instruction[i].immediate;
					if(num>=32768)
						num-=65536;
					int target=num+reg[instruction[i].rs];
					if(check_over(num,reg[instruction[i].rs],target+1))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
					else{
						if(check_over(num,reg[instruction[i].rs],target))
							fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
					}
					if(target+3<1024 && target>=0){
						if(target%2==0){
							int num=(memory[target]<<8)+memory[target+1];
							if(num>=32768)
								num-=65536;
							reg[instruction[i].rt]=num;
							if(reg[instruction[i].rt]!=j)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
						}
						else{
							fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
							mis_ali=true;
						}
					}
					else{
						fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
						addr_over=true;
						if(target%2!=0)
							fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
					}	
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 37:	//lhu
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					int num=instruction[i].immediate;
					if(num>=32768)
						num-=65536;
					int target=num+reg[instruction[i].rs];
					if(check_over(num,reg[instruction[i].rs],target+1))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
					else{
						if(check_over(num,reg[instruction[i].rs],target))
							fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
					}
					if(target+3<1024 && target>=0){
						if(target%2==0){
							int num=(memory[target]<<8)+memory[target+1];
							reg[instruction[i].rt]=num;
							if(reg[instruction[i].rt]!=j)
								fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
						}
						else{
							fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
							mis_ali=true;
						}
					}
					else{
						fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
						addr_over=true;
						if(target%2!=0)
							fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
					}	
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 32:	//lb
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					int num=instruction[i].immediate;
					if(num>=32768)
						num-=65536;
					int target=num+reg[instruction[i].rs];
					if(check_over(num,reg[instruction[i].rs],target))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
					if(target+3<1024 && target>=0){
						int num=memory[target];
						if(num>=128)
							num-=256;
						reg[instruction[i].rt]=num;
						if(reg[instruction[i].rt]!=j)
							fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
					}
					else{
						fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
						addr_over=true;
					}	
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 36:	//lbu
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					int num=instruction[i].immediate;
					if(num>=32768)
						num-=65536;
					int target=num+reg[instruction[i].rs];
					if(check_over(num,reg[instruction[i].rs],target))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
					if(target+3<1024 && target>=0){;
						reg[instruction[i].rt]=memory[target];
						if(reg[instruction[i].rt]!=j)
							fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
					}
					else{
						fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
						addr_over=true;
					}	
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 43:	//sw
				int swtarget;
				int swnum;
				swnum=instruction[i].immediate;
				if(swnum>=32768)
					swnum-=65536;
				swtarget=swnum+reg[instruction[i].rs];
				if(check_over(swnum,reg[instruction[i].rs],swtarget+3))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
				else{
					if(check_over(swnum,reg[instruction[i].rs],swtarget))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
				}
				if(swtarget+3<1024 && swtarget>=0){
					if(swtarget%4==0){
						unsigned int num=reg[instruction[i].rt];
						memory[swtarget]=num>>24;
						num=num<<8>>8;
						memory[swtarget+1]=num>>16;
						num=num<<16>>16;
						memory[swtarget+2]=num>>8;
						num=num<<24>>24;
						memory[swtarget+3]=num;
					}
					else{
						fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
						mis_ali=true;
					}
				}
				else{
					fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
					addr_over=true;
					if(swtarget%4!=0)
						fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
				}
				break;
			case 41:	//sh
				int shtarget;
				int shnum;
				shnum=instruction[i].immediate;
				if(shnum>=32768)
					shnum-=65536;
				shtarget=shnum+reg[instruction[i].rs];
				if(check_over(shnum,reg[instruction[i].rs],shtarget+1))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
				else{
					if(check_over(shnum,reg[instruction[i].rs],shtarget))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);	
				}
				if(shtarget+3<1024 && shtarget>=0){
					if(shtarget%2==0){
						unsigned int num=reg[instruction[i].rt];
						num=num<<16>>16;
						memory[shtarget]=num>>8;
						num=num<<24>>24;
						memory[shtarget+1]=num;
					}
					else{
						fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
						mis_ali=true;
					}
				}
				else{
					fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
					addr_over=true;
					if(shtarget%2!=0)
						fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
				}
				break;
			case 40:	//sb
				int sbtarget;
				int sbnum;
				sbnum=instruction[i].immediate;
				if(sbnum>=32768)
					sbnum-=65536;
				sbtarget=sbnum+reg[instruction[i].rs];
				if(check_over(sbnum,reg[instruction[i].rs],sbtarget))
					fprintf(error,"In cycle %d: Number Overflow\n",cycle);
				if(sbtarget+3<1024 && sbtarget>=0){
					unsigned int num=reg[instruction[i].rt];
					num=num<<24>>24;
					memory[sbtarget+1]=num;
				}
				else{
					fprintf(error,"In cycle %d: Address Overflow\\n",cycle);
					addr_over=true;
				}
				break;
			case 16:	//lui
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					reg[instruction[i].rt]=instruction[i].immediate<<16;
					if(reg[instruction[i].rt]!=j)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 12:	//andi
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					reg[instruction[i].rt]=reg[instruction[i].rs]&instruction[i].immediate;
					if(reg[instruction[i].rt]!=j)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 13:	//ori
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					reg[instruction[i].rt]=reg[instruction[i].rs]|instruction[i].immediate;
					if(reg[instruction[i].rt]!=j)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 14:	//nori
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					reg[instruction[i].rt]=~(reg[instruction[i].rs]|instruction[i].immediate);
					if(reg[instruction[i].rt]!=j)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 10:	//slti
				if(instruction[i].rt!=0){
					int j=reg[instruction[i].rt];
					int target=instruction[i].immediate;
					if(target>=32768)
						target-=65536;
					reg[instruction[i].rt]=(reg[instruction[i].rs]<target);
					if(reg[instruction[i].rt]!=j)
						fprintf(snap,"$%02d: 0x%08X\n",instruction[i].rt,reg[instruction[i].rt]);
				}
				else fprintf(error,"In cycle %d: Write $0 Error\n",cycle);
				break;
			case 4:		//beq
				if(reg[instruction[i].rt]==reg[instruction[i].rs]){
					int a=PC;
					int b=4*instruction[i].immediate;
					PC=a+b;
					if(check_over(a,b,PC))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
				}
				break;
			case 5:		//bne
				if(reg[instruction[i].rt]!=reg[instruction[i].rs]){
					int a=PC;
					int b=4*instruction[i].immediate;
					PC=a+b;
					if(check_over(a,b,PC))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
				}
				break;
			case 7:		//bgtz
				if(reg[instruction[i].rt]>=0){
					int a=PC;
					int b=4*instruction[i].immediate;
					PC=a+b;
					if(check_over(a,b,PC))
						fprintf(error,"In cycle %d: Number Overflow\n",cycle);
				}
				break;
			
			//J_type
			case 2:		//j
				unsigned int jnum;
				jnum=PC;
				jnum=(jnum>>28<<28)+4*(unsigned int)instruction[i].address;
				PC=jnum;
				break;
			case 3:		//jal
				int ra;
				ra=reg[31];
				reg[31]=PC;
				if(ra!=reg[31])
					fprintf(snap,"$31: 0x%08X\n",reg[31]);
				unsigned int jalnum;
				jalnum=PC;
				jalnum=(jalnum>>28<<28)+4*(unsigned int)instruction[i].address;
				PC=jalnum;
				break;
			
			case 63:
				break;
			default:
				//cout<<"error\n";
				break;
		}
    	fprintf(snap,"PC: 0x%08X\n\n\n",PC);
		i=(PC-init)/4;
		
		/*
		if(PC>1023){
			break;
		}
		if(PC<0||PC>end) PC=init;
		*/
		//if(cycle==36) break;
	}
	
	fclose(snap);
	fclose(error);
    return 0;
}
