#include "stdafx.h"

#include "DifferentialAttack.h"
int main()
{
	printf("\t\t————对3轮DES进行差分攻击————\n\n");
	int i;
	TextPair PlainPair[3],CipherPair[3];
	FILE *fin=NULL;
	printf("说明：作为输入的二进制文件\"input.bin\"的应按如下格式组织，否则可能破解失败：\n\n");
	printf("明文11明文12\n明文21明文22\n明文31明文32\n密文11密文12\n密文21密文22\n密文31密文32\n");
	printf("其中明文i1与明文i2的低32位相同(1<=i<=3)。\n\n");
	printf("请按任意键开始破解密钥……\n\n");
	_getch();
	while (1)
	{	
		if (NULL==(fin=fopen("input.bin","rb")))
		{
			printf("打开文件失败，请确保\"input.bin\"文件与程序在同一根下！\n");
			printf("按任意键重试……\n");
			_getch();
			continue;
		}
		break;
	}
	for (i=0;i<3;i++)
	{
		fread(PlainPair[i].First,sizeof(ElemType),8,fin);//读入第i组明文对
		fread(PlainPair[i].Second,sizeof(ElemType),8,fin);
	}
	for (i=0;i<3;i++)
	{
		fread(CipherPair[i].First,sizeof(ElemType),8,fin);//读入第i组密文对
		fread(CipherPair[i].Second,sizeof(ElemType),8,fin);
	}
/*	TextPair PlainPair[3]={{
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
								{0x45,0xFA,0x28,0x5B,0xE5,0xAD,0xC7,0x30},
								{0x13,0x4F,0x79,0x15,0xAC,0x25,0x34,0x57}
							},
							{
								{0xD8,0xA3,0x1B,0x2F,0x28,0xBB,0xC5,0xCF},
								{0x0F,0x31,0x7A,0xC2,0xB2,0x3C,0xB9,0x44}
							}};*/
	ElemType PossibleKey[56]={0};//可能的56位密钥结果
	CrackKey(PlainPair,CipherPair,PossibleKey);//破解密钥，结果保存到PossibleKey中
	if (!SearchKey(PossibleKey,PlainPair[0].First,CipherPair[0].First))
	{
		printf("破解失败！请检查明密文对格式是否正确。\n\n");
	}
	system("pause");
	return 0;
}

void Permutation(ElemType *data,int dataNum,const ElemType permutationTable[])//置换函数
{
	int i;
	ElemType *temp=(ElemType*)malloc(sizeof(ElemType)*dataNum);
	for (i=0;i<dataNum;i++)
	{
		*(temp+i)=permutationTable[i]?*(data+permutationTable[i]-1):'?';//置换表中为0的位置代表不确定，用'?'表示
	}
	memcpy(data,temp,sizeof(ElemType)*dataNum);
	free(temp);
}

void ExtendPermutation(const ElemType data[32],ElemType result[48])//扩展置换
{
	int i;
	for (i=0;i<48;i++)
	{
		result[i]=data[EP[i]-1];
	}
}

void ConstructTestSet(const ElemType E_1[6],const ElemType E_2[6],
	const ElemType C[4],int SBOXNum,ElemType Test[64])//构造测试集合Test(E,E*,C)，参数SBOXNum是S盒序号
{
	ElemType E[6]={0};
	ElemType testE1=0;
	int i,j;
	XOR(E_1,E_2,E,6);			//E'=E xor E*对应此处为E=E_1 xor E_2
	for (testE1=0;testE1<64;testE1++)		//testE1从000000~111111依次尝试
	{
		ElemType E1[6]={0},E2[6]={0};
		ElemType SBoxOutput_E1[4]={0},SBoxOutput_E2[4]={0},SBoxOutputXor[4]={0};
		for (i=0;i<6;i++)
		{
			E1[5-i]=(testE1>>i)&0x01;//将testE1转换为6比特数据保存到E1中
			E2[5-i]=E1[5-i]^E[5-i];//E*=E xor E'此处对应为E2=E1 xor E
		}
		SBOX(E1,SBoxOutput_E1,SBOXNum);
		SBOX(E2,SBoxOutput_E2,SBOXNum);
		XOR(SBoxOutput_E1,SBoxOutput_E2,SBoxOutputXor,4);//S盒输出异或=S(E) xor S(E xor E')，此处对应为
												//SBoxOutputXor=S(SBoxOutput_E1) xor S(SBoxOutput_E2)
		for (i=0;i<4;i++)
		{
			if (SBoxOutputXor[i]!=C[i])
				break;
		}
		if (4==i)//比对成功，当前E1对应的S盒输出异或与C相同
		{
			ElemType byteE_1=0;
			for (j=0;j<6;j++)
			{
				byteE_1=(byteE_1<<1)|E_1[j];//把E_1转化成二进制形式
			}
			Test[byteE_1^testE1]++;//与E_1结果异或得到Test中元素，对应位置计数器加1
		}
	}
}

void SBOX(ElemType input[6],ElemType output[4],int boxNum)
{
	int i;
	ElemType temp=S[boxNum][input[0]*2+input[5]][input[1]*8+input[2]*4+input[3]*2+input[4]];
	for(i=0;temp;i++)					//转换成二进制
	{
		output[3-i]=temp%2;
		temp/=2;
	}
}

void XOR(const ElemType data1[],const ElemType data2[],ElemType result[],int dataNum)//异或函数
{
	int i;
	for(i=0;i<dataNum;i++)
		result[i]=data1[i]^data2[i];
}

void DecToBin(int decNum,ElemType result[],int bitNum)//把十进制数decNum转换成bitNum位二进制存到result中
{
	int i;
	for (i=0;decNum;i++)
	{
		result[bitNum-i-1]=decNum%2;
		decNum/=2;
	}
}

void Ror(ElemType *data,int dataNum,int times)//循环右移函数，将data循环右移times位
{
	ElemType *temp=(ElemType*)malloc(sizeof(ElemType)*times);
	memcpy(temp,data+dataNum-times,sizeof(ElemType)*times);	//保存要循环移动出界的位
	memcpy(data+times,data,sizeof(ElemType)*(dataNum-times));
	memcpy(data,temp,sizeof(ElemType)*times);
	free(temp);
}

void CrackKey(TextPair PlainPair[3],TextPair CipherPair[3],ElemType PossibleKey[56])//密钥破解函数
{
	ElemType EL3_1[3][48]={0},EL3_2[3][48]={0};//扩展后的L3与L3*，二者的异或值是E'
	ElemType SBoxOutput[3][32]={0};//三组明密文对的S盒输出异或，即C'=C xor C* = P^-1(R3' xor L0')
	ElemType Test[8][64]={0};//构造的Test集合
	int i,j;
	for (i=0;i<3;i++)
	{
		for (j=0;j<4;j++)
		{
			ElemType R3=0,L0=0;
			R3=CipherPair[i].First[j+4]^CipherPair[i].Second[j+4];//R3'=R3 xor R3*
			L0=PlainPair[i].First[j]^PlainPair[i].Second[j];//L0'=L0 xor L0*
			ByteToBit(R3^L0,SBoxOutput[i]+j*8);//计算三组R3' xor L0'并转化成bit形式存入SBoxOutput
		}
	}
	for (i=0;i<3;i++)
	{
		Permutation(SBoxOutput[i],32,P_Inverse);//经过P置换的逆置换得到S盒输出异或	
	}
	for (i=0;i<3;i++)
	{
		ElemType L3_1[32],L3_2[32];
		for (j=0;j<4;j++)
		{
			ByteToBit(CipherPair[i].First[j],L3_1+j*8);//将二进制的L3转化成比特形式便于置换
			ByteToBit(CipherPair[i].Second[j],L3_2+j*8);//将二进制的L3*转化成比特形式便于置换
		}
		ExtendPermutation(L3_1,EL3_1[i]);//计算E(L3)
		ExtendPermutation(L3_2,EL3_2[i]);//计算E(L3*)
	}
	for (i=0;i<8;i++)//构造8个集合Test以确定K3
	{
		ConstructTestSet(EL3_1[0]+6*i,EL3_2[0]+6*i,SBoxOutput[0]+4*i,i,Test[i]);
		ConstructTestSet(EL3_1[1]+6*i,EL3_2[1]+6*i,SBoxOutput[1]+4*i,i,Test[i]);
		ConstructTestSet(EL3_1[2]+6*i,EL3_2[2]+6*i,SBoxOutput[2]+4*i,i,Test[i]);
	}
	for (i=0;i<8;i++)
	{
		for (j=0;j<64;j++)
		{
			if (3==Test[i][j])
			{
				DecToBin(j,PossibleKey+i*6,6);//把三组Test集的交集(计数器为3)对应的位置索引转成二进制存到破解密钥中
			}
		}
	}
	Permutation(PossibleKey,56,PC2_Inverse);
	Ror(PossibleKey,28,4);//前28位循环移动4位（第一轮1位，第二轮1位，第三轮2位，共4位）
	Ror(PossibleKey+28,28,4);//后28位循环移动4位
	Permutation(PossibleKey,56,PC1_Inverse);//PC置换的逆置换

}

bool SearchKey(ElemType PossibleKey[54],ElemType PlainText[8],ElemType CipherText[8])
	//枚举可能密钥结果，只要某一结果加密明文PlainText得到的结果与密文CipherText相同则输出
{
	ElemType i;
	for (i=0;i<256;i++)//从00000000b~11111111b枚举2^8=256个未知位
	{
		ElemType Bin[8]={0};
		ElemType tempKey[64]={0};
		ElemType tempCipherText[8]={0};//存储使用当前枚举出的密钥加密过的明文
		DecToBin(i,Bin,8);//转化成二进制存储
		int cnt=0,j;
		for (j=0;j<64;j++)
		{
			if (0==(j+1)%8)//校验位随意填充
			{
				tempKey[j]='?';
			}
			else
			{
				tempKey[j]='?'==PossibleKey[j-j/8]?Bin[cnt++]:PossibleKey[j-j/8];//如果该位被确定就加入tempKey，否则顺序找Bin里的位加入
				//这里PossibleKey的索引要用j-j/8是因为每行会多一位校验位，tempKey的索引要大j/8
			}
		}
		DES_Encrypt(PlainText,tempCipherText,tempKey);//用当前枚举的密钥破解第一组明文，看是否与相应密文相等
		if (0==memcmp(CipherText,tempCipherText,8*sizeof(ElemType)))//如果当前密钥破解的与对应密文相等，则输出这一密钥
		{
			printf("破解出的56位密钥如下，\"?\"代表校验位，对加密无影响。\n");
			for (i=0;i<64;i++)
			{
				if (0==(i+1)%8)
					printf("%c\n",tempKey[i]);
				else 
					printf("%d ",tempKey[i]);
			}
			return 1;
		}
	}
	return 0;
}