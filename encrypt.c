#include <stdio.h>
#include <stdlib.h>
#include "Crypto.h"

byte** sboxm;
byte* encryptionKey;
byte round_keys[11][KEY_BYTES];

void SubWord(byte word[4]){
	word[0] = sboxm[(byte)word[0] >> 4][(byte)(word[0] << 4) >> 4];
	word[1] = sboxm[(byte)word[1] >> 4][(byte)(word[1] << 4) >> 4];
	word[2] = sboxm[(byte)word[2] >> 4][(byte)(word[2] << 4) >> 4];
	word[3] = sboxm[(byte)word[3] >> 4][(byte)(word[3] << 4) >> 4];
}

void RotWord(byte word[4]){
	int temp = word[0];
	word[0] = word[1];
	word[1] = word[2];
	word[2] = word[3];
	word[3] = temp;
}

void ExpandKey(){

	byte RCon[10][4] = {
		{0x01, 0x00, 0x00, 0x00},
		{0x02, 0x00, 0x00, 0x00},
		{0x04, 0x00, 0x00, 0x00},
		{0x08, 0x00, 0x00, 0x00},
		{0x10, 0x00, 0x00, 0x00},
		{0x20, 0x00, 0x00, 0x00},
		{0x40, 0x00, 0x00, 0x00},
		{0x80, 0x00, 0x00, 0x00},
		{0x1B, 0x00, 0x00, 0x00},
		{0x36, 0x00, 0x00, 0x00}
	};

	byte w[44][4];
	int i, j, k;

	for(i=0;i<4;i++){
		w[i][0] = encryptionKey[4*i];
		w[i][1] = encryptionKey[4*i+1];
		w[i][2] = encryptionKey[4*i+2];
		w[i][3] = encryptionKey[4*i+3];
	}

	byte temp[4];

	for(i=4;i<44;i++){
		temp[0] = w[i-1][0];
		temp[1] = w[i-1][1];
		temp[2] = w[i-1][2];
		temp[3] = w[i-1][3];

		if(i%4==0){
			RotWord(temp);
			SubWord(temp);
			for(j=0;j<4;j++){
				temp[j] = temp[j]^RCon[i/4][j];
			}
		}

		for(j=0;j<4;j++){
			w[i][j] = w[i-4][j]^temp[j];
		}
	}

	j = 0;
	for(i=0;i<11;i++){
		j = 0;
		for(k=0;k<4;k++){
			round_keys[i][j++] = w[4*i][k];
		}
		for(k=0;k<4;k++){
			round_keys[i][j++] = w[4*i+1][k];
		}
		for(k=0;k<4;k++){
			round_keys[i][j++] = w[4*i+2][k];
		}
		for(k=0;k<4;k++){
			round_keys[i][j++] = w[4*i+3][k];
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

void SubBytes(byte state[4][4]){

	int i, j;

	SubWord(state[0]);
	SubWord(state[1]);
	SubWord(state[2]);
	SubWord(state[3]);

	// for(i=0;i<4;i++){
	// 	for(j=0;j<4;j++){
	// 		state[i][j] = sboxm[(byte)state[i][j] >> 4][(byte)(state[i][j] << 4) >> 4];
	// 	}
	// }

}

void ShiftRows(byte state[4][4]){

	int temp1, temp2;

	// first row untouched

	// second row left shifted by 1 byte
	temp1 = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp1;

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

void Round(byte state[4][4], int round){

	SubBytes(state);
	ShiftRows(state);
	MixColumns(state);
	AddKey(state, round);

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

	AddKey(state, 0);

	for(i=1;i<=10;i++){
		Round(state, i);
	}

	byte output[16];
	k = 0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			output[k++] = state[j][i];
		}
	}

	for(i=0;i<16;i++){
		printf("%x ", output[i]);
	}
	printf("\n");
}

void main(){
	sboxm = GenerateSBox();
	// byte** invsboxm = GetInvSBox(sboxm);
	encryptionKey = GetKey();

	ExpandKey();

	int i, j;
	// for(i=0;i<16;i++){
	// 	for(j=0;j<16;j++){
	// 		printf("%x ", invsboxm[i][j]);
	// 	}
	// 	printf("\n");
	// }

	// for(i=0;i<KEY_BYTES;i++){
	// 	printf("%x ", encryptionKey[i]);
	// }
	// printf("\n");

	// for(i=0;i<11;i++){
	// 	for(j=0;j<KEY_BYTES;j++){
	// 		printf("%x ", round_keys[i][j]);
	// 	}
	// 	printf("\n");
	// }

	byte input[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	Encrypt(input);

}
