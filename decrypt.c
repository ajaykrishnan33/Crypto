#include <stdio.h>
#include <stdlib.h>
#include "Crypto.h"

byte* decryptionKey;
byte round_keys[10][KEY_BYTES];
byte** invsboxm;
void inverseSubBytes(byte cipherState[4][4])
{
   int i, j;

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			cipherState[i][j] = invsboxm[(byte)cipherState[i][j] >> 4][(byte)(key[i] << 4) >> 4];
		}
	}
}


void AddKey(byte state[4][4], int round){

	byte flat[16];

	int i,j,k=0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			flat[k++] = state[j][i];
		}
	}

	for(i=0;i<16;i++){
		flat[i] = flat[i]^round_keys[round][i];
	}
	
	k = 0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[j][i] = flat[k++];
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
			newstate[i][j] = A[i][0]*cipherState[0][j] + A[i][1]*cipherState[1][j] + A[i][2]*cipherState[2][j] + A[i][3]*cipherState[3][j];
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

	AddKey(state, round);
    inverseMixColumns(cipherState);
    inverseShiftRows(cipherState);
    inverseSubBytes(cipherState);
}

void decrypt(byte cipher[16])
{
   byte cipherState[4][4];
   int i,j,k=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++){
			cipherState[j][i] = cipher[k++];
		}
	}

	for (i = 1; i <= 10; i++) 
	{
		Round(state, i);
	}

	AddKey(state,0);
    byte planetext[16];

	l = 0;
	for (i = 0; i < 4; i++)
    {
		for (j = 0; j < 4; j++) {
			planetext[l++] = state[j][i];
		}
	}
	for (i = 0; i < 16; i++) 
	{
		printf("%x ", planetext[i]);
	}
	printf("\n");
 
}

int main()
{   sboxm = GenerateSBox();
    invsboxm = GetInvSBox(sboxm);
    
	return 0;
}