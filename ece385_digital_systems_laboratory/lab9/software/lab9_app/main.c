/************************************************************************

Lab 9 Nios Software



Dong Kai Wang, Fall 2017

Christine Chen, Fall 2013



For use with ECE 385 Experiment 9

University of Illinois ECE Department

************************************************************************/



#include <stdlib.h>

#include <stdio.h>

#include <time.h>

#include "aes.h"

#include <string.h>



// Pointer to base address of AES module, make sure it matches Qsys

volatile unsigned int * AES_PTR = (unsigned int *) 0x00000040;



// Execution mode: 0 for testing, 1 for benchmarking

int run_mode = 0;



/** charToHex

 *  Convert a single character to the 4-bit value it represents.

 *

 *  Input: a character c (e.g. 'A')

 *  Output: converted 4-bit value (e.g. 0xA)

 */

char charToHex(char c)

{

	char hex = c;



	if (hex >= '0' && hex <= '9')

		hex -= '0';

	else if (hex >= 'A' && hex <= 'F')

	{

		hex -= 'A';

		hex += 10;

	}

	else if (hex >= 'a' && hex <= 'f')

	{

		hex -= 'a';

		hex += 10;

	}

	return hex;

}



/** charsToHex

 *  Convert two characters to byte value it represents.

 *  Inputs must be 0-9, A-F, or a-f.

 *

 *  Input: two characters c1 and c2 (e.g. 'A' and '7')

 *  Output: converted byte value (e.g. 0xA7)

 */

char charsToHex(char c1, char c2)

{

	char hex1 = charToHex(c1);

	char hex2 = charToHex(c2);

	return (hex1 << 4) + hex2;

}



uint subword(uint word){

	uint word_temp, a, b, c, d;



	word_temp=((word&0xFF000000)>>24);

	a=aes_sbox[word_temp]<<24;



	word_temp=((word&0x00FF0000)>>16);

	b=aes_sbox[word_temp]<<16;



	word_temp=((word&0x0000FF00)>>8);

	c=aes_sbox[word_temp]<<8;



	word_temp=word&0x000000FF;

	d=aes_sbox[word_temp];

	return a+b+c+d;

}



uint rotword(uint word){

	uint word_temp1, word_temp2;

	word_temp1=((word&0xFF000000)>>24);

	word_temp2=((word&0x00FFFFFF)<<8);

	return word_temp1+word_temp2;

}



void keyexpansion(uchar* key, uint* word, int nk){

	uint temp;

	int i=0;

	while (i<nk){

		word[i]=(key[4*i]<<24) + (key[4*i+1]<<16) + (key[4*i+2]<<8) + (key[4*i+3]);

		i=i+1;

	}

	i=nk;

	while(i<nb*(nr+1)){

		temp=word[i-1];

		if(i % nk==0){

			temp=subword(rotword(temp))^Rcon[i/nk];

		}

		word[i]=word[i-nk]^temp;

		i=i+1;

	}

	return;

}



void addroundkey(uchar* state, uint* expan, int idx){

	int i;

	uint temp[4];

	int rkey=0;

	int rword=0;

	uint extra;

// 0 3  4 7  8 11

	for(i=4*idx; i<4*idx+4; i++){

		temp[rword]=expan[i];

		rword++;

	}



	rword=0;

	for(i=0; i<16; i++){

		extra=(temp[rword]>>(24-(8*rkey)))& 0xFF;

		state[i] ^= extra;

		rkey++;

		if(rkey>3){

			rkey=0;

			rword++;

		}

	}

	return;

}



void subbytes(uchar* state){

	int i;

	for(i=0; i<16; i++){

		state[i]=aes_sbox[state[i]];

	}

	return;

}



void shiftrows(uchar* state){

uchar temp;



temp=state[1];

state[1]=state[5];

state[5]=state[9];

state[9]=state[13];

state[13]=temp;



temp=state[2];

state[2]=state[10];

state[10]=temp;

temp=state[6];

state[6]=state[14];

state[14]=temp;



temp=state[3];

state[3]=state[15];

state[15]=state[11];

state[11]=state[7];

state[7]=temp;

	return;

}



void mixcolumns(uchar* state){

unsigned char temp[16];

	temp[0] = (gf_mul[state[0]][0] ^ gf_mul[state[1]][1] ^ state[2] ^ state[3]);
	temp[1] = (state[0] ^ gf_mul[state[1]][0] ^ gf_mul[state[2]][1] ^ state[3]);
	temp[2] = (state[0] ^ state[1] ^ gf_mul[state[2]][0] ^ gf_mul[state[3]][1]);
	temp[3] = (gf_mul[state[0]][1] ^ state[1] ^ state[2] ^ gf_mul[state[3]][0]);

	temp[4] = (gf_mul[state[4]][0] ^ gf_mul[state[5]][1] ^ state[6] ^ state[7]);
	temp[5] = (state[4] ^ gf_mul[state[5]][0] ^ gf_mul[state[6]][1] ^ state[7]);
	temp[6] = (state[4] ^ state[5] ^ gf_mul[state[6]][0] ^ gf_mul[state[7]][1]);
	temp[7] = (gf_mul[state[4]][1] ^ state[5] ^ state[6] ^ gf_mul[state[7]][0]);

	temp[8] = (gf_mul[state[8]][0] ^ gf_mul[state[9]][1] ^ state[10] ^ state[11]);
	temp[9] = (state[8] ^ gf_mul[state[9]][0] ^ gf_mul[state[10]][1] ^ state[11]);
	temp[10] = (state[8] ^ state[9] ^ gf_mul[state[10]][0] ^ gf_mul[state[11]][1]);
	temp[11] = (gf_mul[state[8]][1] ^ state[9] ^ state[10] ^ gf_mul[state[11]][0]);

	temp[12] = (gf_mul[state[12]][0] ^ gf_mul[state[13]][1] ^ state[14] ^ state[15]);
	temp[13] = (state[12] ^ gf_mul[state[13]][0] ^ gf_mul[state[14]][1] ^ state[15]);
	temp[14] = (state[12] ^ state[13] ^ gf_mul[state[14]][0] ^ gf_mul[state[15]][1]);
	temp[15] = (gf_mul[state[12]][1] ^ state[13] ^ state[14] ^ gf_mul[state[15]][0]);

	memcpy(state, temp, 16);

	return;

}



/** encrypt

 *  Perform AES encryption in software.

 *

 *  Input: msg_ascii - Pointer to 32x 8-bit char array that contains the input message in ASCII format

 *         key_ascii - Pointer to 32x 8-bit char array that contains the input key in ASCII format

 *  Output:  msg_enc - Pointer to 4x 32-bit int array that contains the encrypted message

 *               key - Pointer to 4x 32-bit int array that contains the input key

 */

void encrypt(unsigned char * msg_ascii, unsigned char * key_ascii, unsigned int * msg_enc, unsigned int * key)

{

	// Implement this function

	uchar state[16];

	uchar key_t[16];

	uchar key_sc[16];

	int i, j;

	for(i=0; i<16; i++){

		state[i]=charsToHex(msg_ascii[2*i], msg_ascii[2*i+1]);

		key_t[i]=charsToHex(key_ascii[2*i], key_ascii[2*i+1]);

	}





	uint key_schedule[4*(nr+1)];

	keyexpansion(key_t, key_schedule, 4);

	addroundkey(state, key_schedule, 0);



	printf("\n");

	for(i=1; i<10; i++){

		subbytes(state);

		shiftrows(state);

		mixcolumns(state);

		addroundkey(state, key_schedule, i);

	}



	subbytes(state);

	shiftrows(state);

	addroundkey(state, key_schedule, 10);



	 for(i=0; i<4; i++){

	 	key_sc[4*i]=(key_schedule[i]&0xFF000000)>>24;

	 	key_sc[4*i+1]=(key_schedule[i]&0x00FF0000)>>16;

	 	key_sc[4*i+2]=(key_schedule[i]&0x0000FF00)>>8;

	 	key_sc[4*i+3]=(key_schedule[i]&0x000000FF);

	 } //key expansion



	for(i=0; i<4; i++){

		msg_enc[i]=(state[i*4]<<24)|(state[i*4+1]<<16)|(state[i*4+2]<<8)|state[i*4+3];
		AES_PTR[i]=(key_t[i*4]<<24)|(key_t[i*4+1]<<16)|(key_t[i*4+2]<<8)|key_t[i*4+3];
		AES_PTR[i+4]=msg_enc[i];
	}


}





/** decrypt

 *  Perform AES decryption in hardware.

 *

 *  Input:  msg_enc - Pointer to 4x 32-bit int array that contains the encrypted message

 *              key - Pointer to 4x 32-bit int array that contains the input key

 *  Output: msg_dec - Pointer to 4x 32-bit int array that contains the decrypted message

 */

void decrypt(unsigned int * msg_enc, unsigned int * msg_dec, unsigned int * key)

{

	// Implement this function
	int i;
	AES_PTR[14]=1;
	while(AES_PTR[15]==0){
		};
	for(i=0; i<4; i++){
		msg_dec[i]=AES_PTR[i+8];
	}

	AES_PTR[14]=0;

	return;
}



/** main

 *  Allows the user to enter the message, key, and select execution mode

 *

 */

int main()

{

	// Input Message and Key as 32x 8-bit ASCII Characters ([33] is for NULL terminator)

	unsigned char msg_ascii[33]; //ece298dcece298dcece298dcece298dc

	unsigned char key_ascii[33]; //000102030405060708090a0b0c0d0e0f

	// Key, Encrypted Message, and Decrypted Message in 4x 32-bit Format to facilitate Read/Write to Hardware

	unsigned int key[4];

	unsigned int msg_enc[4];

	unsigned int msg_dec[4];



	printf("Select execution mode: 0 for testing, 1 for benchmarking: ");

	scanf("%d", &run_mode);



	if (run_mode == 0) {

		//Continuously Perform Encryption and Decryption

		while (1) {

			int i = 0;

			printf("\nEnter Message:\n");

			scanf("%s", msg_ascii);

			printf("\n");

			printf("\nEnter Key:\n");

			scanf("%s", key_ascii);

			printf("\n");



			encrypt(msg_ascii, key_ascii, msg_enc, key);

			printf("\nEncrpted message is: \n");



			for(i = 0; i < 4; i++){

				printf("%08x", msg_enc[i]);

			}

			printf("\n");

			decrypt(msg_enc, msg_dec, key);

			printf("\nDecrypted message is: \n");

			for(i = 0; i < 4; i++){

				printf("%08x", msg_dec[i]);

			}

			printf("\n");

			return 0;

		}

	}

	else {

		// Run the Benchmark

		int i = 0;

		int size_KB = 2;

		// Choose a random Plaintext and Key

		for (i = 0; i < 32; i++) {

			msg_ascii[i] = 'a';

			key_ascii[i] = 'b';

		}

		// Run Encryption

		clock_t begin = clock();

		for (i = 0; i < size_KB * 64; i++)

			encrypt(msg_ascii, key_ascii, msg_enc, key);

		clock_t end = clock();

		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		double speed = size_KB / time_spent;

		printf("Software Encryption Speed: %f KB/s \n", speed);

		// Run Decryption

		begin = clock();

		for (i = 0; i < size_KB * 64; i++)

			decrypt(msg_enc, msg_dec, key);

		end = clock();

		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		speed = size_KB / time_spent;

		printf("Hardware Encryption Speed: %f KB/s \n", speed);

	}

	return 0;

}



// int main(){

// 	// uint x[]={0x0c0d0e0f};

// 	// printf("%x\n",*x);

// 	// uint y=rotword(x);

// 	// printf("%x\n",y);

// 	// uint temp=subword(y);

// 	// printf("%x\n", temp);

// 	// printf("%x\n", Rcon[1]);

// 	// uint b= temp^Rcon[1];

// 	// printf("%x\n", b);

// 	uint x[]={0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f};

// 	printf("%x\n", x);

// 	return 0;

// }

