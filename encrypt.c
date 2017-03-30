#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Crypto.h"

byte** sboxm;
byte* encryptionKey;
byte** round_keys;

void SubBytes(byte state[4][4]) {

	int i, j;

	SubWord(sboxm, state[0]);
	SubWord(sboxm, state[1]);
	SubWord(sboxm, state[2]);
	SubWord(sboxm, state[3]);

}

void ShiftRows(byte state[4][4]) {

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

void MixColumns(byte state[4][4]) {

	int i, j;

	byte A[4][4] = {
		{2, 3, 1, 1},
		{1, 2, 3, 1},
		{1, 1, 2, 3},
		{3, 1, 1, 2}
	};

	byte newstate[4][4];

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			newstate[i][j] = A[i][0] * state[0][j] + A[i][1] * state[1][j] + A[i][2] * state[2][j] + A[i][3] * state[3][j];
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[i][j] = newstate[i][j];
		}
	}

}

void Round(byte state[4][4], int round) {

	SubBytes(state);
	ShiftRows(state);
	MixColumns(state);
	AddKey(state, round_keys[round]);

}

void Encrypt(byte input[16]) {
	byte state[4][4];

	sboxm = GenerateSBox();

	int i, j, k = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] = input[k++];
		}
	}

	AddKey(state, round_keys[0]);

	for (i = 1; i <= 10; i++) {
		Round(state, i);
	}

	byte output[16];
	k = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			output[k++] = state[j][i];
		}
	}

	for (i = 0; i < 16; i++) {
		printf("%x ", output[i]);
	}
	printf("\n");
}

// void EncryptCBC(char* input){

// 	int len = strlen(input);

// 	int padded_len = len % 16 ? len + 16 : ( len / 16 ) * 16 + 16;

// 	byte* padded_input = calloc(padded_len, sizeof(byte));

// 	int i;
// 	for(i=0;i<len;i++){
// 		padded_input[i] = (byte)input[i];
// 	}

// 	int d = padded_len - len - 16;

// 	padded_input[padded_len-1] = 

// 	for(i=padded_len-16;i<padded_len;i++){

// 	}

// }

void main() {
	sboxm = GenerateSBox();
	// byte** invsboxm = GetInvSBox(sboxm);
	encryptionKey = GetKey();

	round_keys = ExpandKey(sboxm, encryptionKey);

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
