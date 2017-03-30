#include <stdio.h>
#include <stdlib.h>
#include "Crypto.h"

byte* encryptionKey;
byte** round_keys;
byte** invsboxm;
byte** sboxm;
void inverseSubBytes(byte cipherState[4][4])
{
   int i, j;

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			cipherState[i][j] = invsboxm[(byte)cipherState[i][j] >> 4][(byte)(cipherState[i][j] << 4) >> 4];
		}
	}
}

void inverseMixColumns(byte cipherState[4][4])
{
  int i, j;

	byte A[4][4] = {
		{0xe, 0xb, 0xd, 0x9},
		{0x9, 0xe, 0xb, 0xd},
		{0xd, 0x9, 0xe, 0xb},
		{0xb, 0xd, 0x9, 0xe}
	};

	byte newstate[4][4];


	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			newstate[i][j] = (A[0][j]*cipherState[i][0] + A[1][j]*cipherState[i][1] + A[2][j]*cipherState[i][2] + A[3][j]*cipherState[i][3])%256;
		}
	}

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			cipherState[i][j] = newstate[i][j];
		}
	}

}

void inverseShiftRows(byte cipherState[4][4])
{
    int temp1, temp2;

	// first row untouched

	// second row right shifted by 1 byte
	temp1 = cipherState[1][3];
	cipherState[1][3] = cipherState[1][2];
	cipherState[1][2] = cipherState[1][1];
	cipherState[1][1] = cipherState[1][0];
	cipherState[1][0] = temp1;

	// second row right shifted by 2 bytes
	temp1 = cipherState[2][2];
	temp2 = cipherState[2][3];
	cipherState[2][3] = cipherState[2][1];
	cipherState[2][2] = cipherState[2][0];
	cipherState[2][1] = temp2;
	cipherState[2][0] = temp1;

	// third row right shifted by 4 bytes
	temp1 = cipherState[3][0];
	cipherState[3][0] = cipherState[3][1];
	cipherState[3][1] = cipherState[3][2];
	cipherState[3][2] = cipherState[3][3];
	cipherState[3][3] = temp1;
}

void Round(byte cipherState[4][4], int round) {

	AddKey(cipherState, round_keys[round]);
    //inverseMixColumns(cipherState);
    inverseShiftRows(cipherState);
    inverseSubBytes(cipherState);
}

byte *decrypt(byte cipher[16])
{
   byte cipherState[4][4];
   int i,j,l=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++){
			cipherState[j][i] = cipher[l++];
		}
	}

	for (i = 10; i > 0; i--) 
	{
		Round(cipherState, i);
	}

	AddKey(cipherState, round_keys[0]);
    static byte planetext[16];

	l = 0;
	for (i = 0; i < 4; i++)
    {
		for (j = 0; j < 4; j++) {
			planetext[l++] = cipherState[j][i];
		}
	}
	for (i = 0; i < 16; i++) 
	{
		printf("%x ", planetext[i]);
	}
	printf("\n");
    return planetext;
}


byte *cbc_decrypt(char* cipher)
{   int len = strlen(cipher);
	byte* output = calloc(len, sizeof(byte));
	byte IV[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	byte* temp = IV;
	byte inter[16];
    byte inter2;
	int k=0;
	int i,j;
	int iter=len/16;
	for(i=0;i<iter;i++)
	{
       for(j=0;j<16;j++){
			inter[j] = cipher[16*i+j];
		}
		byte* out=decrypt(inter);
		for(j=0;j<16;j++)
		{
           //inter2=inter[j];
           out[j]=out[j]^temp[j];
           temp[j]=inter[j];
		}
		for(j=0;j<16;j++){
			output[k++] = out[j];
		}
	}
  return output;
}
int main()
{   sboxm = GenerateSBox();
    invsboxm = GetInvSBox(sboxm);
    encryptionKey = GetKey();

	round_keys = ExpandKey(sboxm, encryptionKey);

    byte input[16] = {0x39, 0x33, 0xb, 0xa9, 0xad, 0x7f, 0x7a, 0x59, 0x6a, 0x49, 0x7b, 0x2f, 0x7b, 0x17, 0xb, 0xef};

    decrypt(input);
	return 0;
}
