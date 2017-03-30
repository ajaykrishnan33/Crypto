#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Crypto.h"

byte encryptionKey[KEY_BYTES] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
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

byte fieldMult(byte val, byte x){
	byte result = 0;
	switch(val){
		case 1:
			result = x;
			break;
		case 2:
			result = (byte)x << 1;
			if(x>127)
				result = result^0x1b;
			break;
		case 3:
			result = fieldMult(x, 2);
			result = result^x;
			break;
		case 9:
			result = fieldMult(x, 2);
			result = fieldMult(result, 2);
			result = fieldMult(result, 2);
			result = result ^ x;
			break;
		case 11:
			result = fieldMult(x, 2);
			result = fieldMult(result, 2);
			result = result^x;
			result = fieldMult(result, 2);
			result = result^x;
			break;
		case 13:
			result = fieldMult(x, 2);
			result = result^x;
			result = fieldMult(result, 2);
			result = fieldMult(result, 2);
			result = result^x;
			break;
		case 14:
			result = fieldMult(x, 2);
			result = result^x;
			result = fieldMult(result, 2);
			result = result^x;
			result = fieldMult(result, 2);
			break;
	}
	return result;
}

void inverseMixColumns(byte state[4][4])
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
			newstate[i][j] = fieldMult(A[i][0], state[0][j]) ^ fieldMult(A[i][1], state[1][j]) ^ fieldMult(A[i][2], state[2][j]) ^ fieldMult(A[i][3], state[3][j]);
		}
	}

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[i][j] = newstate[i][j];
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

	sboxm = GenerateSBox(round_keys[round]);
    invsboxm = GetInvSBox(sboxm);
	AddKey(cipherState, round_keys[round]);
    // inverseMixColumns(cipherState);
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
    return planetext;
}

byte *cbc_decrypt(byte* cipher, int iterations, int* length)
{   
	byte* output = calloc(iterations*16, sizeof(byte));
	byte IV[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	byte* temp = IV;
	byte inter[16];
	int k=0;
	int i,j;
	for(i=0;i<iterations;i++)
	{
       for(j=0;j<16;j++){
			inter[j] = cipher[16*i+j];
		}
		byte* out=decrypt(inter);
		for(j=0;j<16;j++)
		{
           out[j]=out[j]^temp[j];
           temp[j]=inter[j];
		}
		for(j=0;j<16;j++){
			output[k++] = out[j];
		}
	}

	int d = (int)output[k-1];
	*length = iterations*16 - 16 - d;

  return output;
}

int main()
{   sboxm = GenerateSBox(encryptionKey);
    invsboxm = GetInvSBox(sboxm);
	round_keys = ExpandKey(sboxm, encryptionKey);

	int i;

    char inp_str[35000];
    fgets(inp_str, 35000, stdin);

    int x = strlen(inp_str)/3;

    byte* input = malloc(sizeof(byte)*x);

    const char delim[2] = " ";
    char* tok = strtok(inp_str, delim);

    unsigned int val;

    i = 0;
    while(tok!=NULL){
    	sscanf(tok, "%x", &val);
    	input[i++] = (byte)val;
    	tok = strtok(NULL, " ");
    }

    int len=0;
    byte* p = cbc_decrypt(input, x/16, &len);

    for (i = 0; i < len; i++)
	{
		printf("%c", p[i]);
	}
	return 0;
}
