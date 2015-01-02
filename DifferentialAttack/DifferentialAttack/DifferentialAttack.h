#ifndef DIFFRENTIALATTACK_H
#define DIFFRENTIALATTACK_H
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "DES_3Round.h"
typedef struct textpair			//明文或者密文对
{
	ElemType First[8];
	ElemType Second[8];
}TextPair;
const ElemType P_Inverse[]={ 9,17,23,31,13,28, 2,18,			//P置换的逆置换
	24,16,30, 6,26,20,10, 1,
	8,14,25, 3, 4,29,11,19,
	32,12,22, 7, 5,27,15,21};
const ElemType PC1_Inverse[]={					//PC-1置换的逆置换
								8,16,24,56,52,44,36,
								7,15,23,55,51,43,35,
								6,14,22,54,50,42,34,
								5,13,21,53,49,41,33,
								4,12,20,28,48,40,32,
								3,11,19,27,47,39,31,
								2,10,18,26,46,38,30,
								1, 9,17,25,45,37,29};

const ElemType PC2_Inverse[]={					//PC-2置换的逆置换，位置上是0的代表压缩置换中未出现的位
	5,24, 7,16, 6,10,20,18,
	0,12, 3,15,23, 1, 9,19,
	2, 0,14,22,11, 0,13, 4,
	0,17,21, 8,47,31,27,48,
	35,41, 0,46,28, 0,39,32,
	25,44, 0,37,34,43,29,36,
	38,45,33,26,42, 0,30,40};
void XOR(const ElemType data1[],const ElemType data2[],ElemType result[],int dataNum);//异或函数
void Permutation(ElemType *data,int dataNum,const ElemType permutationTable[]);//置换函数
void ExtendPermutation(const ElemType data[32],ElemType result[48]);//扩展置换
void SBOX(ElemType input[6],ElemType output[4],int boxNum);//S盒
void ConstructTestSet(const ElemType E_1[6],const ElemType E_2[6],
const ElemType C[4],int SBOXNum,ElemType Test[64]);//构造测试集合Test(E,E*,C)，参数SBOXNum是S盒序号
void DecToBin(int decNum,ElemType result[],int bitNum);//把十进制数decNum转换成bitNum位二进制存到result中
void Ror(ElemType *data,int dataNum,int times);//循环右移函数，将data循环右移times位
void CrackKey(TextPair PlainPair[3],TextPair CipherPair[3],ElemType PossibleKey[56]);//密钥破解函数
bool SearchKey(ElemType PossibleKey[54],ElemType PlainText[8],ElemType CipherText[8]);
	//枚举可能密钥结果，只要某一结果加密明文PlainText得到的结果与密文CipherText相同则输出
#endif