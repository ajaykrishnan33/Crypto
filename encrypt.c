#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Crypto.h"

byte** sboxm;
byte encryptionKey[KEY_BYTES] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
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

byte fieldMult(byte val2, byte val1){
	byte result = 0;
	switch(val2){
		case 1:
			result = val1;
			break;
		case 2:
			result = (byte)val1 << 1;
			if(val1>127)
				result = result^0x1b;
			break;
		case 3:
			result = fieldMult(val1, 2);
			result = result^val1;
			break;
	}
	return result;
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
			newstate[i][j] = fieldMult(A[i][0], state[0][j]) ^ fieldMult(A[i][1], state[1][j]) ^ fieldMult(A[i][2], state[2][j]) ^ fieldMult(A[i][3], state[3][j]);
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[i][j] = newstate[i][j];
		}
	}

}

void Round(byte state[4][4], int round) {

	sboxm = GenerateSBox(round_keys[round]);
	SubBytes(state);
	ShiftRows(state);
	// MixColumns(state);
	AddKey(state, round_keys[round]);

}

byte* Encrypt(byte input[16]) {
	byte state[4][4];

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

	byte* output = malloc(sizeof(byte)*16);
	k = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			output[k++] = state[j][i];
		}
	}

	return output;
}

byte* EncryptCBC(char* input, int* length){

	int len = strlen(input);

	int padded_len = len % 16 == 0 ? len + 16 : ( len / 16 ) * 16 + 32;

	byte* padded_input = calloc(padded_len, sizeof(byte));
	byte* output = calloc(padded_len, sizeof(byte));

	int i, j, k;
	for(i=0;i<len;i++){
		padded_input[i] = (byte)input[i];
	}

	int d = padded_len - len - 16;

	padded_input[padded_len-1] = (byte)d;

	int max_iter = padded_len/16;

	byte IV[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	byte* temp = IV;
	byte inter[16];
	k = 0;
	for(i=0;i<max_iter;i++){
		for(j=0;j<16;j++){
			inter[j] = padded_input[16*i+j]^temp[j];
		}
		temp = Encrypt(inter);
		for(j=0;j<16;j++){
			output[k++] = temp[j];
		}
	}
	*length = k;

	return output;

}

void main(int argc, char* argv[]) {
	sboxm = GenerateSBox(encryptionKey);
	round_keys = ExpandKey(sboxm, encryptionKey);

	int i;

	
	char filename[256];
	// scanf("%s", filename);

	FILE* fp;
	fp = fopen(argv[1], "r");

	char input[10000];
	fread(input, sizeof(char), 10000, fp);

	int x = 0;
	byte* output = EncryptCBC(input, &x);

	for (i = 0; i < x; i++) {
		printf("%02x ", output[i]);
	}
	printf("\n");


}
