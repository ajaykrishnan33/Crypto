#include <stdio.h>
#include <stdlib.h>
#include "Crypto.h"

void SubWord(byte** sboxm, byte word[4]) {
	word[0] = sboxm[(byte)word[0] >> 4][(byte)(word[0] << 4) >> 4];
	word[1] = sboxm[(byte)word[1] >> 4][(byte)(word[1] << 4) >> 4];
	word[2] = sboxm[(byte)word[2] >> 4][(byte)(word[2] << 4) >> 4];
	word[3] = sboxm[(byte)word[3] >> 4][(byte)(word[3] << 4) >> 4];
}

void RotWord(byte word[4]) {
	int temp = word[0];
	word[0] = word[1];
	word[1] = word[2];
	word[2] = word[3];
	word[3] = temp;
}

byte** ExpandKey(byte** sboxm, byte* encryptionKey) {

	byte** round_keys = malloc(sizeof(byte*)*11);

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

	for (i = 0; i < 4; i++) {
		w[i][0] = encryptionKey[4 * i];
		w[i][1] = encryptionKey[4 * i + 1];
		w[i][2] = encryptionKey[4 * i + 2];
		w[i][3] = encryptionKey[4 * i + 3];
	}

	byte temp[4];

	for (i = 4; i < 44; i++) {
		temp[0] = w[i - 1][0];
		temp[1] = w[i - 1][1];
		temp[2] = w[i - 1][2];
		temp[3] = w[i - 1][3];

		if (i % 4 == 0) {
			RotWord(temp);
			SubWord(sboxm, temp);
			for (j = 0; j < 4; j++) {
				temp[j] = temp[j] ^ RCon[i / 4][j];
			}
		}

		for (j = 0; j < 4; j++) {
			w[i][j] = w[i - 4][j] ^ temp[j];
		}
	}

	j = 0;
	for (i = 0; i < 11; i++) {
		round_keys[i] = malloc(sizeof(byte)*KEY_BYTES);
		j = 0;
		for (k = 0; k < 4; k++) {
			round_keys[i][j++] = w[4 * i][k];
		}
		for (k = 0; k < 4; k++) {
			round_keys[i][j++] = w[4 * i + 1][k];
		}
		for (k = 0; k < 4; k++) {
			round_keys[i][j++] = w[4 * i + 2][k];
		}
		for (k = 0; k < 4; k++) {
			round_keys[i][j++] = w[4 * i + 3][k];
		}
	}

	for(i=0;i<KEY_BYTES;i++){
		round_keys[10][i] = round_keys[0][i];
	}

	return round_keys;

}

void AddKey(byte state[4][4], byte* key) {

	byte flat[16];

	int i, j, k = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			flat[k++] = state[j][i];
		}
	}

	for (i = 0; i < 16; i++) {
		flat[i] = flat[i] ^ key[i];
	}

	k = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] = flat[k++];
		}
	}

}