#include <stdio.h>
#include <stdlib.h>
#include "Crypto.h"

void SubBytes(){

}

void ShiftRows(){

}

void MixColumns(){

}

void Encrypt(char* input){

	char state[4][4];
	// byte

}

void main(){
	byte** sboxm = GenerateSBox();
	int i, j;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			printf("%x ",sboxm[i][j]);
		}
		printf("\n");
	}
}
