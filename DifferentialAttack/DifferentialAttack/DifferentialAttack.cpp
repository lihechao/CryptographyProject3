// DifferentialAttack.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
typedef struct textpair			//明文或者密文对
{
	unsigned char First[8];
	unsigned char Second[8];
}TextPair;
const int EP[]={32, 1, 2, 3, 4, 5,							//扩展置换
				 4, 5, 6, 7, 8, 9,
				 8, 9,10,11,12,13,
				12,13,14,15,16,17,
				16,17,18,19,20,21,
				20,21,22,23,24,25,
				24,25,26,27,28,29,
				28,29,30,31,32, 1};
const int P_Inv[]={  9,17,23,31,13,28, 2,18,			//P置换的逆置换
					24,16,30, 6,26,20,10, 1,
					 8,14,25, 3, 4,29,11,19,
					32,12,22, 7, 5,27,15,21};
/* S盒 */
const int S[8][4][16] =/* S1 */
{{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
/* S2 */
{{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
/* S3 */
{{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
/* S4 */
{{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
/* S5 */
{{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
/* S6 */
{{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
/* S7 */
{{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
/* S8 */
{{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};
void ExtendPermutation(int data[32],int result[48]);
void Permutation(int *data,int dataNum,const int permutationFun[]);
void ByteToBit(const unsigned char byte[],int bit[],int byteNum);
void XOR(int data1[],int data2[],int result[],int dataNum);
void SBOX(int input[6],int output[4],int boxNum);
void IN(int input[6],int output[4],int test[64]);
int main()
{
//	FILE *fin=NULL;
	int i,j;
	TextPair PlainPair[3]={{
							{0x74,0x85,0x02,0xCD,0x38,0x45,0x10,0x97},
							{0x38,0x74,0x75,0x64,0x38,0x45,0x10,0x97}
							},
						   {
							{0x48,0x69,0x11,0x02,0x6A,0xCD,0xFF,0x31},
							{0x37,0x5B,0xD3,0x1F,0x6A,0xCD,0xFF,0x31}
							},
						   {
							{0x35,0x74,0x18,0xDA,0x01,0x3F,0xEC,0x86},
							{0x12,0x54,0x98,0x47,0x01,0x3F,0xEC,0x86}
							}};
	TextPair CipherPair[3]={{
							 {0x03,0xC7,0x03,0x06,0xD8,0xA0,0x9F,0x10},
							 {0x78,0x56,0x0A,0x09,0x60,0xE6,0xD4,0xCB}
							},
							{
							 {0x45,0xFA,0x28,0xBE,0x5A,0xDC,0x73,0x00},
							 {0x13,0x4F,0x79,0x15,0xAC,0x25,0x34,0x57}
							},
							{
							 {0xD8,0xA3,0x1B,0x2F,0x28,0xBB,0xC5,0xCF},
							 {0x0F,0x31,0x7A,0xC2,0xB2,0x3C,0xB9,0x44}
							}};
//	fin=fopen("input.bin","rb");

//	fclose(fin);
	int L3_1[32]={0},E_1[48]={0};
	int L3_2[32]={0},E_2[48]={0};
	int Test[8][64]={0};
	int E[48]={0};
	unsigned char R3[4]={0},L0[4]={0},R3XORL0[4];
	int R3XORL0Bit[32],L0Bit[32];
	ByteToBit(CipherPair[0].First,L3_1,4);
	ExtendPermutation(L3_1,E_1);
	ByteToBit(CipherPair[0].Second,L3_2,4);
	ExtendPermutation(L3_2,E_2);
	for (i=0;i<48;i++)
	{
		E[i]=(E_1[i]+E_2[i])%2;
	}
	for (j=4;j<8;j++)
	{
		R3[j-4]=CipherPair[0].First[j]^CipherPair[0].Second[j];
	}
	for (j=0;j<4;j++)
	{
		L0[j]=PlainPair[0].First[j]^PlainPair[0].Second[j];
	}
	for (j=0;j<4;j++)
	{
		R3XORL0[j]=R3[j]^L0[j];
	}
	ByteToBit(R3XORL0,R3XORL0Bit,4);
	Permutation(R3XORL0Bit,32,P_Inv);
	for (i=0;i<32;i++)
	{
		printf("%d ",R3XORL0Bit[i]);
	}
	IN(E,R3XORL0Bit,Test[0]);
	printf("\n");
	for (i=0;i<4;i++)
	{
		for (j=0;j<16;j++)
		{
			printf("%d ",Test[0][i*16+j]);
		}
		printf("\n");
	}
	return 0;
}

void ByteToBit(const unsigned char byte[],int bit[],int byteNum)
{
	int i,j;
	for (i=0;i< byteNum;i++)
	{
		for (j=0;j<8;j++)
		{
			bit[i*8+j]=((byte[i]<<j)&0x80)>>7;
		}
	}
}

void Permutation(int *data,int dataNum,const int permutationFun[])
{
	int i,*temp=(int*)malloc(sizeof(int)*dataNum);
	for (i=0;i<dataNum;i++)
	{
		*(temp+i)=*(data+permutationFun[i]-1);
	}
	memcpy(data,temp,sizeof(int)*dataNum);
}

void ExtendPermutation(int data[32],int result[48])
{
	int i;
	for (i=0;i<48;i++)
	{
		result[i]=data[EP[i]-1];
	}
}

void IN(int input[6],int output[4],int test[64])
{
	int i,temp[8];
	unsigned char t=0;
	for (t=0;t<64;t++)
	{
		int tempInput[6]={0};
		int tempOutput[4]={0};
		int tp[4]={0};
		ByteToBit(&t,temp,1);
		XOR(temp+2,input,tempInput,6);
		SBOX(tempInput,tempOutput,0);
		SBOX(temp+2,tp,0);
		XOR(tp,tempOutput,tempOutput,6);
		for (i=0;i<4;i++)
		{
			if (tempOutput[i]!=output[i])
				break;
		}
		if (4==i)
		{
			test[t]++;
		}
	}
}

void XOR(int data1[],int data2[],int result[],int dataNum)			//异或函数
{
	int i;
	for(i=0;i<dataNum;i++)
	{
		result[i]=data1[i]^data2[i];
	}
}

void SBOX(int input[6],int output[4],int boxNum)
{
	int i,temp=S[boxNum][input[0]*2+input[5]][input[1]*8+input[2]*4+input[3]*2+input[4]];
	for(i=0;temp;i++)					//转换成二进制
	{
		output[3-i]=temp%2;
		temp/=2;
	}
}