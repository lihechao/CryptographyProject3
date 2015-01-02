#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
const int IP[]={1,5,2,0,3,7,4,6};			//初始置换
const int IP_Inv[]={3,0,2,4,6,1,7,5};			//初始逆置换
const int P10[]={2,4,1,6,3,9,0,8,7,5};		//P10
const int P8[]={5,2,6,3,7,4,9,8};			//P8
const int EP[]={3,0,1,2,1,2,3,0};			//扩展置换
const int S[2][4][4]={{{1,0,3,2},				//S盒S0
						{3,2,1,0},
						{0,2,1,3},
						{3,1,3,2}},
						{{0,1,2,3},				//S盒S1			
						{2,0,1,3},
						{3,0,1,0},
						{2,1,0,3}}};
const int P4[]={1,3,2,0};
void Permutation(int *data,int dataNum,const int permutationTable[]);	//通用的置换函数，可用于IP、IP逆、P4、P10
void ExtendPermutation(const int data[4],int result[8]);				//扩展置换
void Permutation8(const int data[10],int result[8]);					//P8置换
void XOR(int data1[],int data2[],int result[],int dataNum);			//异或函数
void Rol(int *data,int dataNum,int times);				//循环左移函数，其中dataNum是要移动的数据数目，times是左移次数
void CreateSubKey(int key[10],int subKey[2][8]);			//密钥生成函数，其中key是输入密钥，subKey保存生成的全部子密钥
void ByteToBit(const char byte,int bit[8]);				//字节转换成比特，bit[0]~bit[7]存储byte的高位到低位
void BitToByte(const int bit[8],char *byte);			//比特转换成字节
void SBOX(int input[4],int output[2],int boxNum);		//S盒，其中boxNum代表第几个S盒
void EncryptByte(char *byte,int subKey[2][8]);			//加密一字节数据
void DecryptByte(char *byte,int subKey[2][8]);			//解密一字节数据
void EncryptFile(FILE *PlainFile,FILE *EncryptFile,int Key[10]);			//加密文件
void DecryptFile(FILE *EncryptFile,FILE *PlainFile,int Key[10]);			//解密文件
int main()
{
	printf("\t\t——————S-DES加解密工具——————\n\n");
	while(1)
	{
		char OpNum,FileName[255]={0};
		FILE *fin=NULL,*fout=NULL;
		int i,Key[10]={0};
		printf("请选择要进行的操作：1-加密 2-解密 3-退出\n");
		do
		{
			OpNum=_getch();
		}while(!('1'==OpNum||'2'==OpNum||'3'==OpNum));
		switch(OpNum)
		{
		case '1':
			while (1)
			{
				printf("\n请输入要加密的文件名（包括扩展名）：");
				scanf("%s",FileName);
				if (NULL==(fin=fopen(FileName,"rb")))
				{
					printf("\n打开文件失败，请重试！\n");
					continue;
				}
				break;
			}
			while (1)
			{
				printf("\n请输入要输出结果的文件名（包括扩展名）：");
				scanf("%s",FileName);
				if (NULL==(fout=fopen(FileName,"wb")))
				{
					printf("\n创建文件失败，请重试！\n");
					continue;
				}
				break;
			}
			while(1)
			{
				printf("\n请输入10bit加密密钥：");
				fflush(stdin);//清空缓冲区
				for (i=0;i<10;i++)
				{
					scanf("%c",&Key[i]);
					if (!('0'==Key[i]||'1'==Key[i]))
						break;
					Key[i]-='0';
				}
				if (10==i)
					break; 
				else
				{
					printf("\n您输入的密钥有误，请重新输入！\n");
				}
			}
			EncryptFile(fin,fout,Key);
			fclose(fin);
			fclose(fout);
			printf("\n加密结果已输出至文件%s中\n\n",FileName);
			break;
		case '2':
			while (1)
			{
				printf("\n请输入要解密的文件名（包括扩展名）：");
				scanf("%s",FileName);
				if (NULL==(fin=fopen(FileName,"rb")))
				{
					printf("\n打开文件失败，请重试！\n");
					continue;
				}
				break;
			}
			while (1)
			{
				printf("\n请输入要输出结果的文件名（包括扩展名）：");
				scanf("%s",FileName);
				if (NULL==(fout=fopen(FileName,"wb")))
				{
					printf("\n创建文件失败，请重试！\n");
					continue;
				}
				break;
			}
			while(1)
			{
				printf("\n请输入10bit加密密钥：");
				fflush(stdin);
				for (i=0;i<10;i++)
				{
					scanf("%c",&Key[i]);
					if (!('0'==Key[i]||'1'==Key[i]))
						break;
					Key[i]-='0';
				}
				if (10==i)
					break; 
				else
				{
					printf("\n您输入的密钥有误，请重新输入！\n");
				}
			}
			DecryptFile(fin,fout,Key);
			fclose(fin);
			fclose(fout);
			printf("\n解密结果已输出至文件%s中\n\n",FileName);
			break;
		case '3':
			exit(0);
			break;
		default:
			break;
		}
	}
	int a[]={1,0,1,0,0,0,0,0,1,0};
	int b[2][8];
	CreateSubKey(a,b);
	for (int i=0;i<2;i++)
	{
		for(int j=0;j<8;j++)
			printf("%d ",b[i][j]);
		printf("\n");
	}
	return 0;
}

void Permutation(int *data,int dataNum,const int permutationTable[])
{
	int i,*temp=(int*)malloc(sizeof(int)*dataNum);
	for (i=0;i<dataNum;i++)
	{
		*(temp+i)=*(data+permutationTable[i]);
	}
	memcpy(data,temp,sizeof(int)*dataNum);
	free(temp);
}

void ExtendPermutation(const int data[4],int result[8])
{
	int i;
	for (i=0;i<8;i++)
	{
		result[i]=data[EP[i]];
	}
}

void Permutation8(const int data[10],int result[8])
{
	int i;
	for (i=0;i<8;i++)
	{
		*(result+i)=*(data+P8[i]);
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

void Rol(int *data,int dataNum,int times)
{
	int *temp=(int *)malloc(sizeof(int)*times);
	memcpy(temp,data,sizeof(int)*times);					//保存要循环移动的位
	memcpy(data,data+times,sizeof(int)*(dataNum-times));
	memcpy(data+dataNum-times,temp,sizeof(int)*times);
	free(temp);
}

void CreateSubKey(int key[10],int subKey[2][8])				//密钥生成函数，其中key是输入密钥，subKey保存生成的全部子密钥
{
	Permutation(key,10,P10);		//P10置换

	Rol(key,5,1);					//密钥高五位循环左移一位
	Rol(key+5,5,1);					//密钥低五位循环左移一位
	Permutation8(key,subKey[0]);	//P8置换得到子密钥K1

	Rol(key,5,2);					//密钥高五位循环左移两位
	Rol(key+5,5,2);					//密钥低五位循环左移两位
	Permutation8(key,subKey[1]);	//P8置换得到子密钥K2
}

void ByteToBit(const char byte,int bit[8])
{
	int i;
	for(i=0;i<8;i++)
	{
		bit[7-i]=(byte>>i)&0x01;
	}
}
void BitToByte(const int bit[8],char *byte)
{
	int i;
	*byte=0;
	for(i=0;i<8;i++)
	{
		*byte=(*byte<<1)|bit[i];
	}
}

void SBOX(int input[4],int output[2],int boxNum)
{
	int i,temp=S[boxNum][input[0]*2+input[3]][input[1]*2+input[2]];
	output[0]=output[1]=0;
	for(i=0;temp;i++)					//转换成二进制
	{
		output[2-i-1]=temp%2;
		temp/=2;
	}
}

void EncryptByte(char *byte,int subKey[2][8])
{
	int i,j;
	int bit[8]={0},Left[4]={0},Right[4]={0},ER[8]={0},temp[4]={0};
	ByteToBit(*byte,bit);				//把一字节数据转换成8比特数据，进行后续操作
	Permutation(bit,8,IP);				//进行初始置换
	memcpy(Left,bit,sizeof(int)*4);		//得到4位的左半部分Left
	memcpy(Right,bit+4,sizeof(int)*4);	//得到4位的右半部分Right

	for(i=0;i<2;i++)
	{
		ExtendPermutation(Right,ER);		//右半部分先进行扩展置换得到8位ER
		XOR(ER,subKey[i],ER,8);				//将ER与子密钥进行异或，结果存到ER中
		for(j=0;j<2;j++)
		{
			SBOX(ER+j*4,temp+j*2,j);		//进入S盒
		}
		Permutation(temp,4,P4);
		XOR(temp,Left,Left,4);				//将左半部分与S盒输出的结果异或并将结果存到Left中
		if (1!=i)
		{
			memcpy(temp,Left,sizeof(int)*4);		//左右部分交换
			memcpy(Left,Right,sizeof(int)*4);
			memcpy(Right,temp,sizeof(int)*4);
		}
	}
	memcpy(bit,Left,sizeof(int)*4);			//最终左右两部分合成
	memcpy(bit+4,Right,sizeof(int)*4);
	Permutation(bit,8,IP_Inv);				//初始置换逆置换
	BitToByte(bit,byte);
}

void DecryptByte(char *byte,int subKey[2][8])
{
	int i,j;
	int bit[8]={0},Left[4]={0},Right[4]={0},ER[8]={0},temp[4]={0};
	ByteToBit(*byte,bit);				//把一字节数据转换成8比特数据，进行后续操作
	Permutation(bit,8,IP);				//进行初始置换
	memcpy(Left,bit,sizeof(int)*4);		//得到4位的左半部分Left
	memcpy(Right,bit+4,sizeof(int)*4);	//得到4位的右半部分Right

	for(i=1;i>=0;i--)
	{
		ExtendPermutation(Right,ER);		//右半部分先进行扩展置换得到8位ER
		XOR(ER,subKey[i],ER,8);				//将ER与子密钥进行异或，结果存到ER中
		for(j=0;j<2;j++)
		{
			SBOX(ER+j*4,temp+j*2,j);		//进入S盒
		}
		Permutation(temp,4,P4);
		XOR(temp,Left,Left,4);				//将左半部分与S盒输出的结果异或并将结果存到Left中
		if (0!=i)
		{
			memcpy(temp,Left,sizeof(int)*4);		//左右部分交换
			memcpy(Left,Right,sizeof(int)*4);
			memcpy(Right,temp,sizeof(int)*4);
		}
	}
	memcpy(bit,Left,sizeof(int)*4);			//最终左右两部分合成
	memcpy(bit+4,Right,sizeof(int)*4);
	Permutation(bit,8,IP_Inv);				//初始置换逆置换
	BitToByte(bit,byte);
}

void EncryptFile(FILE *PlainFile,FILE *EncryptFile,int Key[10])
{
	char byte=0;
	int SubKey[2][8]={0};
	CreateSubKey(Key,SubKey);			//生成子密钥
	while(1)
	{
		byte=fgetc(PlainFile);				//从文件读取一字节数据
		if (EOF==byte)	break;
		EncryptByte(&byte,SubKey);
		fputc(byte,EncryptFile);				//将加密结果写入文件
	}
}

void DecryptFile(FILE *EncryptFile,FILE *PlainFile,int Key[10])
{
	char byte=0;
	int SubKey[2][8]={0};
	CreateSubKey(Key,SubKey);			//生成子密钥
	while(1)
	{
		byte=fgetc(EncryptFile);				//从文件读取一字节数据
		if (EOF==byte)	break;
		DecryptByte(&byte,SubKey);				//解密一个字节
		fputc(byte,PlainFile);				//将解密结果写入文件
	}
}