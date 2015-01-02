#include "stdafx.h"
#include "DES_3Round.h"

void ByteToBit(const unsigned char byte,ElemType bit[8])//将一字节数据转换成比特形式
{
	int i;
	for(i=0;i<8;i++)
	{
		bit[7-i]=(byte>>i)&0x01;
	}
}

void BitToByte(const ElemType Bit[8],unsigned char *Byte)//将8位数据转换成一个字节
{
	int i;
	for (i=0;i<8;i++)
	{
		(*Byte)=(*Byte<<1)|Bit[i];
	}
}

void Rol(ElemType *data,int dataNum,int times)
{
	ElemType *temp=(ElemType *)malloc(sizeof(ElemType)*times);
	memcpy(temp,data,sizeof(ElemType)*times);					//保存要循环移动到右边的位
	memcpy(data,data+times,sizeof(ElemType)*(dataNum-times));
	memcpy(data+dataNum-times,temp,sizeof(ElemType)*times);
	free(temp);
}

void XOR(ElemType data1[],ElemType data2[],ElemType result[],int dataNum)			//异或函数
{
	int i;
	for(i=0;i<dataNum;i++)
	{
		result[i]=data1[i]^data2[i];
	}
}

void Swap(ElemType *Data1,ElemType *Data2,int DataNum)//交换Data1与Data2
{
	ElemType *temp=(ElemType*)malloc(sizeof(ElemType)*DataNum);
	memcpy(temp,Data1,sizeof(ElemType)*DataNum);
	memcpy(Data1,Data2,sizeof(ElemType)*DataNum);
	memcpy(Data2,temp,sizeof(ElemType)*DataNum);
	free(temp);
}

void UniversalPermutation(ElemType *data,int dataNum,const ElemType permutationTable[])//通用置换函数
{
	int i;
	ElemType *temp=(ElemType*)malloc(sizeof(ElemType)*dataNum);
	for (i=0;i<dataNum;i++)
	{
		*(temp+i)=*(data+permutationTable[i]-1);
	}
	memcpy(data,temp,sizeof(ElemType)*dataNum);
	free(temp);
}

void CompressExtendPermutation(ElemType Data[],ElemType Result[],
	const ElemType PermutationTable[],int ResultNum)//压缩/扩展置换函数，其中resultNum是压缩/扩展后的位数
{
	int i;
	for (i=0;i<ResultNum;i++)
	{
		Result[i]=Data[PermutationTable[i]-1];
	}
}

void CreateSubKey(ElemType Key[64],ElemType SubKey[ROUNDNUM][48])//子密钥生成函数
{
	int i;
	ElemType Key_56bit[56]={0};//经过PC-1置换的56比特密钥
	CompressExtendPermutation(Key,Key_56bit,PC_1,56);//PC-1置换
	for (i=0;i<ROUNDNUM;i++)
	{
		Rol(Key_56bit,28,MoveTimes[i]);//前28位循环左移
		Rol(Key_56bit+28,28,MoveTimes[i]);//后28位循环左移
		CompressExtendPermutation(Key_56bit,SubKey[i],PC_2,48);//PC-2置换
	}
}

void SBOX(ElemType Input[48],ElemType Output[32])//S盒，输入48位，输出32位
{
	int i;
	for (i=0;i<8;i++)
	{
		int Row,Col;//S盒的行和列
		ElemType tempOutput=0;
		Row=Input[i*6]*2+Input[i*6+5];
		Col=Input[i*6+1]*8+Input[i*6+2]*4+Input[i*6+3]*2+Input[i*6+4];
		tempOutput=S[i][Row][Col];
		Output[i*4]=(tempOutput&0x08)>>3;			//转换成比特形式
		Output[i*4+1]=(tempOutput&0x04)>>2;
		Output[i*4+2]=(tempOutput&0x02)>>1;
		Output[i*4+3]=tempOutput&0x01;
	}
}

void DES_Encrypt(unsigned char PlainText[8],unsigned char CipherText[8],
	ElemType Key[64])//使用DES加密64位数据（8字节），忽略初始置换和初始逆置换
{
	int i;
	ElemType SubKey[ROUNDNUM][48]={0};
	ElemType PlainBit[64]={0};//比特形式的明文数据
	ElemType ER[48]={0};//经过扩展置换后的右半部分明文
	ElemType SboxOutput[32]={0};//S盒的输出
	CreateSubKey(Key,SubKey);//生成子密钥
	for (i=0;i<8;i++)
	{
		ByteToBit(PlainText[i],PlainBit+i*8);//将明文转化成比特形式方便操作
	}
	for (i=0;i<ROUNDNUM;i++)
	{
		CompressExtendPermutation(PlainBit+32,ER,EP,48);//对明文右半部分进行扩展置换
		XOR(ER,SubKey[i],ER,48);//将扩展置换后结果ER与子密钥进行异或，结果保存在ER中
		SBOX(ER,SboxOutput);//将扩展置换后结果ER输入S盒得到输出SboxOutput
		UniversalPermutation(SboxOutput,32,P);//对S盒输出的结果进行P置换
		XOR(SboxOutput,PlainBit,PlainBit,32);//将明文左半部分与P置换结果进行异或，结果保存到明文左半部分
		Swap(PlainBit,PlainBit+32,32);//交换明文左右部分
	}
	for (i=0;i<8;i++)
	{
		BitToByte(PlainBit+i*8,CipherText+i);//最终结果转换成字节数据
	}
}