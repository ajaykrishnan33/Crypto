#include <stdio.h>
#include <stdlib.h>
#include "Crypto.h"

byte** sboxm;
byte* encryptionKey;
byte round_keys[10][KEY_BYTES];

void ExpandKey(){



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

void SubBytes(byte state[4][4]){

	int i, j;

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[i][j] = sboxm[(byte)state[i][j] >> 4][(byte)(encryptionKey[i] << 4) >> 4];
		}
	}

}

void ShiftRows(byte state[4][4]){

	int temp1, temp2;

	// first row untouched

	// second row left shifted by 1 byte
	temp1 = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp2;

	// second row left shifted by 2 bytes
	temp1 = state[2][0];
	temp2 = state[2][1];
	state[2][0] = state[2][2];
	state[2][1] = state[2][3];
	state[2][2] = temp1;
	state[2][3] = temp2;

	// third row left shifted by 4 bytes
	temp1 = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = state[3][0];
	state[3][0] = temp1;
}

void MixColumns(byte state[4][4]){

	int i, j;

	byte A[4][4] = {
		{2, 3, 1, 1},
		{1, 2, 3, 1},
		{1, 1, 2, 3},
		{3, 1, 1, 2}
	};

	byte newstate[4][4];

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			newstate[i][j] = A[i][0]*state[0][j] + A[i][1]*state[1][j] + A[i][2]*state[2][j] + A[i][3]*state[3][j];
		}
	}

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[i][j] = newstate[i][j];
		}
	}

}

void Encrypt(byte input[16]){
	byte state[4][4];
	
	sboxm = GenerateSBox();

	int i,j,k=0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[j][i] = input[k++];
		}
	}

}

void main(){
	sboxm = GenerateSBox();
	byte** invsboxm = GetInvSBox(sboxm);
	encryptionKey = GetKey();
	int i, j;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			printf("%x ", invsboxm[i][j]);
		}
		printf("\n");
	}
}
