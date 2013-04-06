/*
 ============================================================================
 Name     		   : Catarina Moura
 Student Number    : 340964
 Description 	   : Offset Code Book
 ============================================================================
 */


#include <stdint.h>
#include <stdlib.h>
#include<stdio.h>
#include<string.h>
#include "aes128ocb.h"
#include "aes128e.h"

/* Returns the exponent of the msb of 'value' */
static unsigned int msb(unsigned int value);

/* Returns the number of trailing zeros in 'value' */
static unsigned int ntz(unsigned int value);


unsigned char L[16];

int vector=0 ;


/*
 *********************************************************************************
 ===================================	AES128OCB	==============================
 *********************************************************************************
 */

/* Under the 16-byte (128-bit) key at k and the 12-byte (96-bit) nonce at n,
 * encrypt the plaintext at p and store it at c.
 * The length of the plaintext is a multiple of 16 bytes given at len (e.g., len = 2 for a 32-byte p).
 * The length of the ciphertext c is (len+1)*16 bytes. */
void aes128ocb(unsigned char *c, const unsigned char *k, const unsigned char *n, const unsigned char *p, const unsigned int len) {

	unsigned char L_star[16];
	unsigned char L_dollar[16];
	int i,j; /* Auxiliary variables */

	puts(" ");

	unsigned char zero_str[16];
	/* Places a string of 128 bits starting in the position 0
	 * and store in zero_str. */
	zeros(zero_str,0,128);

	/* L_STAR
	 * Encrypt the function made of 16-byte of zeros and store it at L using the Key */

	aes128e(L_star,zero_str,k);
	//test_lstar(L_star);


	/* L_DOLLAR
	 * The return string will store in L_dollar */
	double_function(L_star, L_dollar, 16);
	//test_ldollar(L_dollar);

	/*The number of plaintext blocks is limited to fit in one unsigned int.
	One unsigned int is in the range 0 to 65535, to know how many L's will
	generate to the messages we will use msb function .
	The maximum of messages could possible have is 15
	because msb(65535) = 15 so you can generate L_0 to L_15 */

	unsigned int L_max = msb(len);
	unsigned char L[L_max+1][16];
	unsigned char *L_temp;


	/* L[0]
	 * The return string will store in L[0] */
	double_function(L_dollar, L[0], 16);
	//test_lm(L[0],0);


	if(L_max != 0){
		/* Compute all L's from L[1] to L[L_max] */
		for(int i =1 ; i <= L_max ; i++){
			double_function(L[i-1], L[i], 16) ;
			//test_lm(L[i],i);
		}
	}

	/* NONCE
	 *  Given 96 bit(12 bytes) of nonce
	 *  Will compute 16 bytes and store Nonce.
	 *  3 bytes(0x00) + 1byte(0x01) + 12bytes(nonce) */

	unsigned char *Nonce = (unsigned char *) malloc(16);
	nonce_function(n,12,Nonce);

	/* BOTTOM
	 * bottom = Nonce[123..128 bits]
	 * So will compute 6 bits (122...127)
	 * byte fifteen */

	int bottom = Nonce[15]& 0x3F ;
	//printf("bottom:%d\n",bottom);

	/* KTOP */

	unsigned char Ktop[16];
	/* Stores the output on Ktop */
	memset((void *) Ktop,(int)'\0',16);
	Ktop_function(Nonce,Ktop,16,k);
	//test_Ktop(Ktop);

	/* STRETCH */

	unsigned char Stretch[24];
	Stretch_function(Ktop,16,Stretch);
//	test_Stretch(Stretch);

	/* OFFSET AND CHECKSUM */

	unsigned char Offset[len+1][16];
	unsigned char Checksum[len+1][16];


	/* OFFSET0 && Checksum0
	 *  Offset0 = Stretch[bottom ... 127+bottom]
	 * Will compute the shift_left and store in Offset[0]
	 *  Will compute the zeros and store in Checksum[0] */

	unsigned char plaintext_xor[16] ;

	shift_left(Stretch,Offset[0],16,bottom,0);
	//test_Offset(Offset[0],0);
	zeros(Checksum[0],0,128);

	for(i=0; i<len; i++){

		for(j=0; j< 16 ; j++){

			Offset[i+1][j] = Offset[i][j] ^ L[ntz(i+1)][j];
			plaintext_xor[j] = p[i*16+j]^ Offset[i+1][j];
		}

		aes128e(&c[i*16], (const unsigned char*)plaintext_xor , k);

		for(j=0; j< 16 ; j++){

			c[i*16+j] = Offset[i+1][j] ^ c[i*16+j];
			Checksum[i+1][j] = Checksum[i][j] ^ p[i*16+j];
		}
	}

//	test_Offset(Offset[0],0);
//	test_Offset(Offset[1],1);
//	test_Offset(Offset[2],2);
//	test_Checksum(Checksum[0],0);
//	test_Checksum(Checksum[1],1);
//	test_Checksum(Checksum[2],2);
//	test_Cipher(c,0);
//	test_Cipher(&c[16],1);

	/* TAG */

	unsigned char Tag[16];

	memset((void *)plaintext_xor,(int)'\0',16);
	for(i=0 ; i<16 ; i++){
		plaintext_xor[i] =Checksum[len][i] ^ Offset[len][i] ^ L_dollar[i] ;
	}
	aes128e(Tag,plaintext_xor,k);

	//test_Tag(Tag);

	/* CIPHER
	 * c= 16 bytes cipher1 + 16 bytes cipher2 + 16 bytes Tag */

		for(i=0; i<16; i++){
			c[len*16+i] = Tag[i];
		}

	//printf("\nVector[%d]:",vector++);
}



/*
 *********************************************************************************
 ===================================	MSB		==================================
 *********************************************************************************
 */
/* Returns the exponent of the msb of 'value' */
static unsigned int msb(unsigned int value) {

	unsigned int index=0;

	while (value >>= 1) { 		/* Loop while greater than one */ 
		index++;
	}
	return index;
}

/*
 *********************************************************************************
 ===================================	NTZ		==================================
 *********************************************************************************
 */

/* Returns the number of trailing zeros in 'value' */
static unsigned int ntz(unsigned int value) {

	unsigned int zeros=0;

	while (!(value & 0x01)) { 	/* Loop while the lsb is zero */
		value >>= 1;				/* Shift to the right; that is, observe the next bit. */
		zeros++;
	}
	return zeros;

}

/*
 *********************************************************************************
 ====================================	ZEROS	==================================
 *********************************************************************************
 */

/* The zeros function will receive the number of bits and bit_start_pos
 * and will store on zero_string made of x zero-bits */

void zeros(unsigned char* zero_string,int bit_start_pos,int number_of_bits){


	/* The length in bytes to store a string of zeros*/
	int length_bytes = number_of_bits/8;
	int start_string_pos= bit_start_pos/8;


	/* Making the string of zero bytes */

	for(int i=start_string_pos ; i < (start_string_pos+length_bytes) ; i++){
		zero_string[i] = 0x00;
	}

	if(number_of_bits%8 != 0){
		zero_string[start_string_pos+length_bytes] >>= number_of_bits%8;
	}
}

/*
 *********************************************************************************
 =================================	DOUBLE 	======================================
 *********************************************************************************
 */

void double_function(unsigned char* L_init,unsigned char* L_dest,int length){

	/* Shift all the bits of the string to the left one bit*/
	shift_left(L_init,L_dest,length,1,1);

	/*L_init[0] & 0x80 - Compute the most significant bit */
	if( (L_init[0] & 0x80) != 0){
		/* The last position of the string xor 0x87 */
		L_dest[length -1] ^= 0x87 ;
	}
}

/*
 *********************************************************************************
 =================================	SHIFT LEFT 	==================================
 *********************************************************************************
 */

/* Shift all the bits of the L_init string to the left x positions bits
 * and store the result on L_dest
 * The parameter equal_length is a boolean that tells the L_init going
 * to have the same length or not.0- != length , 1 - == length  */

void shift_left(unsigned char* L_init,unsigned char* L_dest,int length_byte,int start_pos_bit,int equal_length){

	/* The string_position will give the position of the string to
	 * start doing the shift. Dividing by 8 because is 8 bits
	 * (1 byte = char). Eg: If you want to shift 18 positions
	 * you will start on position number two of the string */
	int one_byte = 8 ;
	int string_position = start_pos_bit/one_byte ;
	int msbits_of_byte = start_pos_bit%one_byte;
	int lsbits_of_byte = one_byte - (start_pos_bit%one_byte);

	/* Shift left x bits on the string */
	for(int i = 0; i<length_byte; i++){

	if(equal_length){
		/* Stopping Condition
		 *  When you do a Shift_left, the last bits of the array will be fill by zeros.
		 *  Just do this if L_init and L_dest have the same length
		 *  otherwise L_dest will not have space to fill at the end with zeros.*/
		if ( i+ string_position == (length_byte-1)   ){

			if(start_pos_bit%8 != 0){
				L_dest[i] =  L_init[i+string_position] << msbits_of_byte ;

				if(string_position != 0){
					/* Starting in i+1 L_dest will be put with zeros */
					zeros(L_dest,(i+1)*8,string_position*8);
				}
			}else{
				/* Will fill L_dest with zeros starting in i */
				zeros(L_dest,i*8,string_position*8);
			}
			return ;
		}
	}
		L_dest[i] = L_init[i+string_position] <<   msbits_of_byte |
				L_init[i+string_position+1] >> lsbits_of_byte ;
	}
}

/*
 *********************************************************************************
 ====================================	NONCE	==================================
 *********************************************************************************
 */

/* Receive the nonce and the length of the nonce in bytes
 * Return the Nonce computed (16 bytes).
 *  3 bytes(0x00) + 1byte(0x01) + 12bytes(nonce) */
void nonce_function(const unsigned char* nonce,int nonce_length,unsigned char* Nonce){
	int i;
	int zeros_length = 16 - nonce_length - 1;

	for(i=0; i<zeros_length ; i++){
		Nonce[i]= 0x00 ;
	}
	Nonce[zeros_length] = 0x01;

	for(i=0; i < nonce_length; i++){
		Nonce[zeros_length+1+i] = nonce[i];
	}

}

/*
 *********************************************************************************
 ===================================	KTOP		==============================
 *********************************************************************************
 */

/* Stores the computation of the Ktop_function on Ktop
 * The expresssion is given by Ktop = ENCIPHER(K,Nonce[0...121]||zeros(6))  */
void Ktop_function(unsigned char* Nonce,unsigned char* Ktop,int length,const unsigned char *key){

	/* Computes the first 15 bytes of nonce 0 to 129 bits */
	for(int i=0; i<15 ; i++ ){
		Ktop[i] = Nonce[i] ;
	}

	/* Computes the two bits remaining, 120,121 */

	Ktop[15] = Nonce[15] & 0xC0 ;
	/* Computes 6 bits of zeros 122 to 127 */
	zeros(Nonce,122,6);

	/* Will store the encryption in Ktop */
	aes128e(Ktop,Ktop,key);
}

/*
 *********************************************************************************
 ===================================	STRETCH		==============================
 *********************************************************************************
 */

/* Stretch = Ktop || (Ktop[0...63] xor Ktop[8...71])
 * Stretch will have 16+8 = 24 bytes*/
void Stretch_function(unsigned char* Ktop,int length_Ktop,unsigned char* Stretch){

	unsigned char stretch_nonce1[8];
	unsigned char stretch_nonce2[8];
	unsigned char stretch_nonce_xor[8];
	int i;

	for(i=0; i < length_Ktop; i++ ){
		Stretch[i] = Ktop[i];
	}


	shift_left(Ktop,stretch_nonce1,8,0,0);
	//printf("stretch_nonce1[1] = 0x%02x\n",stretch_nonce1[1]);

	shift_left(Ktop,stretch_nonce2,8,8,0);
	//printf("stretch_nonce2[1] = 0x%02x\n",stretch_nonce2[1]);

	for(i= 0; i < 8 ; i++){

		stretch_nonce_xor[i] = stretch_nonce1[i] ^ stretch_nonce2[i] ;

		Stretch[length_Ktop+i] = stretch_nonce_xor[i];
	}
}

/*
 *********************************************************************************
 =================================	Print 	======================================
 *********************************************************************************
 */
/* Print all the bits of the string in binary */
void print_bit_string(unsigned char* string_init,int length){

	unsigned char string_aux[length];
	int i ;

	printf("String_Length= %d\n",length);
	/* i - scroll the string */
	for(i =0 ; i < length; i++){

		/* Just copy the string_init to auxiliary string
		 *  to don't mess the values of string_init */
		string_aux[i] = string_init[i];

		printf("String[%d]: 0x%02x (16) = ",i,string_aux[i]);

		/* j- scroll the 8 bits of the position of the string
		 * j=0 MSB , j=7 LSB  */
		for(int j = 0 ;j< 8; j++) {

			if ( string_aux[i] & 0x80){
				printf("1");
			}
			else{
				printf("0");
			}

			/* Tab between the first 4bits and the last 4bits*/
			if(j == 3){
				printf(" ");
			}
			/* shift left one bit that is one zero on the right */
			string_aux[i] <<= 1;
		}
		printf("(2)\n");
	}
	puts(" ");
}

void print_hexa_string(unsigned char* string,int length){

	printf("String_Length= %d\n",length);

		for(int i =0 ; i < length; i++){
			printf("String[%d]: 0x%02x (16)\n",i,string[i]);
		}
}

void print_hexa_string_continuous(unsigned char* string,int length){

	//printf("String_Length= %d\n",length);

		for(int i =0 ; i < length; i++){
			printf("%02x",string[i]);
		}
}


/*
 *********************************************************************************
 =================================	Testing 	==================================
 *********************************************************************************
 */

/* Function to test if the shift left works corrrectly
 * Shift Left one position
 * INPUT = {0x80,0xff,0x66,0x9d}
 * OUTPUT= {0x01,0xfe,0xcd,0x3a} */

void test_double(){

	unsigned char L[]={0x80,0xff,0x66,0x9d};
	//unsigned char L[]={0x00,0xff,0x66,0x9d};

	printf("INPUT: ");
	print_hexa_string(L,4);

	//double_function(L,4);
	//printf("OUTPUT: ");
	//print_hexa_string(L,4);


}

void compare_hex_string(unsigned char* string_result,unsigned char* string_test,int length,char* text){


	for(int i =0 ; i < length; i++){

		if(string_result[i] == string_test[i]){
			continue ;
		}else{
			printf("%s: NOT EQUAL to the output\n",text);
			printf("Failed on vector[%d] string[%d]\n",vector,i);
			printf("%s%d[%d]_program:0x%02x (16)\n",text,vector,i,string_result[i]);
			printf("%s%d[%d]_test:0x%02x (16)\n",text,vector,i,string_test[i]);
			return;
		}

	}

	printf("%s[%d]: OK\n",text,vector);

}

void test_lstar(unsigned char* L_star){

	if(vector == 0){

		unsigned char Lstar_v0[16] = {
				0xc6,0xa1,0x3b,0x37,0x87,0x8f,0x5b,0x82,
				0x6f,0x4f,0x81,0x62,0xa1,0xc8,0xd8,0x79 };

		compare_hex_string(L_star,Lstar_v0,16,"LSTAR");
	}
}

void test_ldollar(unsigned char* L_dollar){

	if(vector == 0){

		unsigned char Ldollar_v0[16] = {
				0x8d,0x42,0x76,0x6f,0x0f,0x1e,0xb7,0x04,
				0xde,0x9f,0x02,0xc5,0x43,0x91,0xb0,0x75 };

		//print_hexa_string_continuous(L_dollar,16);
		compare_hex_string(L_dollar,Ldollar_v0,16,"L$");
	}
}


void test_lm(unsigned char* L_m,int m){

	if(vector == 0 && m==0){

		unsigned char L_v0_m0[16] = {
				0x1a,0x84,0xec,0xde,0x1e,0x3d,0x6e,0x09,
				0xbd,0x3e,0x05,0x8a,0x87,0x23,0x60,0x6d };

		compare_hex_string(L_m,L_v0_m0,16,"L_0");
	}


	if(vector == 0 && m==1){

		unsigned char L_v0_m1[16] = {
				0x35,0x09,0xd9,0xbc,0x3c,0x7a,0xdc,0x13,
				0x7a,0x7c,0x0b,0x15,0x0e,0x46,0xc0,0xda };

		compare_hex_string(L_m,L_v0_m1,16,"L_1");
	}


	if(vector == 0 && m> 1){
		printf("ERROR on L_%d: max of L on the V[0] is L_1",m);
	}
}

void test_Ktop(unsigned char* Ktop){

	if(vector == 0){

			unsigned char Ktop_v0[16] = {
					0x43,0xe1,0x11,0x49,0x8c,0x05,0x82,0xbf,
					0x99,0xf1,0xd9,0x66,0xce,0xfc,0xbc,0xc6 };

			//print_hexa_string_continuous(Ktop,16);
			compare_hex_string(Ktop,Ktop_v0,16,"Ktop");
		}


	if(vector == 1){

			unsigned char Ktop_v1[16] = {
					0x80,0xc4,0x38,0xe0,0x50,0x51,0x4e,0x4d,
					0xe8,0xe3,0x5f,0x81,0x08,0xc1,0x9b,0x46 };

			//print_hexa_string_continuous(Ktop,16);
			compare_hex_string(Ktop,Ktop_v1,16,"Ktop");
		}


}

void test_Stretch(unsigned char* Stretch){

	if(vector == 0){

			unsigned char Stretch_v0[24] = {
					0x43,0xe1,0x11,0x49,0x8c,0x05,0x82,0xbf,
					0x99,0xf1,0xd9,0x66,0xce,0xfc,0xbc,0xc6,
					0xa2,0xf0,0x58,0xc5,0x89,0x87,0x3d,0x26};

			compare_hex_string(Stretch,Stretch_v0,24,"Stretch");
		}

	if(vector == 1){

			unsigned char Stretch_v1[24] = {
					0x80,0xc4,0x38,0xe0,0x50,0x51,0x4e,0x4d,
					0xe8,0xe3,0x5f,0x81,0x08,0xc1,0x9b,0x46,
					0x44,0xfc,0xd8,0xb0,0x01,0x1f,0x03,0xa5};

			compare_hex_string(Stretch,Stretch_v1,24,"Stretch");
		}


}

void test_Offset(unsigned char* Offset,int pos){

	if(vector == 0 && pos==0){

		unsigned char Offset0_v0[16] = {
				0x08,0x8a,0x4c,0x60,0x2c,0x15,0xfc,0xcf,
				0x8e,0xcb,0x36,0x77,0xe5,0xe6,0x35,0x17 };

		//print_hexa_string_continuous(Offset0,16);
		compare_hex_string(Offset,Offset0_v0,16,"Offset0");
}

	if(vector == 0 && pos==1){

		unsigned char Offset1_v0[16] = {
				0x12,0x0e,0xa0,0xbe,0x32,0x28,0x92,0xc6,
				0x33,0xf5,0x33,0xfd,0x62,0xc5,0x55,0x7a };

		compare_hex_string(Offset,Offset1_v0,16,"Offset1");
	}

	if(vector == 0 && pos==2){

			unsigned char Offset2_v0[16] = {
					0x27,0x07,0x79,0x02,0x0e,0x52,0x4e,0xd5,
					0x49,0x89,0x38,0xe8,0x6c,0x83,0x95,0xa0 };

			compare_hex_string(Offset,Offset2_v0,16,"Offset2");
		}
}

void test_Checksum(unsigned char* Checksum,int pos){

	if(vector == 0 && pos==0){

		unsigned char Checksum0_v0[16] = {
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

		compare_hex_string(Checksum,Checksum0_v0,16,"Checksum0");
}
	if(vector == 0 && pos==1){

		unsigned char Checksum1_v0[16] = {
				0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
				0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };

		compare_hex_string(Checksum,Checksum1_v0,16,"Checksum1");
	}
	if(vector == 0 && pos==2){

			unsigned char Checksum2_v0[16] = {
					0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
					0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10 };

			compare_hex_string(Checksum,Checksum2_v0,16,"Checksum2");
		}
}

void test_Cipher(unsigned char* Cipher,int pos){

	if(vector == 0 && pos==0){

		unsigned char Cipher1_v0[16] = {
				0xbe,0xa5,0xe8,0x79,0x8d,0xbe,0x71,0x10,
				0x03,0x1c,0x14,0x4d,0xa0,0xb2,0x61,0x22 };

		compare_hex_string(Cipher,Cipher1_v0,16,"Cipher1");
}
	if(vector == 0 && pos==1){

		unsigned char Cipher2_v0[16] = {
				0xce,0xaa,0xb9,0xb0,0x5d,0xf7,0x71,0xa6,
				0x57,0x14,0x9d,0x53,0x77,0x34,0x63,0xcb };

		compare_hex_string(Cipher,Cipher2_v0,16,"Cipher2");
	}
}

void test_Tag(unsigned char* Tag){

	if(vector == 0){

		unsigned char Tag_v0[16] = {
				0x4a,0x3b,0xae,0x82,0x44,0x65,0xcf,0xda,
				0xf8,0xc4,0x1f,0xc5,0x0c,0x7d,0xf9,0xd9 };

		compare_hex_string(Tag,Tag_v0,16,"Tag");
	}
}



/*
 *********************************************************************************
 =================================	DEPRECATED 	==================================
 *********************************************************************************
 */


/* Shift all the bits of the string to the left x positions bit
 * and store the result on L_dest*/

//void shift_left(unsigned char* L_init, int length,unsigned char* L_dest, int pos){
//
//	unsigned char aux , aux2 ;
//	aux = 0x00 ;
//
//	if( !(pos > 0 && pos<8)){
//
//
//
//		exit();
//	}
//
//	/* The last position of the array (length-1)
//	 * aux saves the most significant bit/s  */
//
//	for(int i = length -1 ; i >= 0 ; i--){
//
//		/* Copy the string L_init to string L_dest */
//		L_dest[i] = L_init[i];
//
//
//		switch(pos){
//
//		case 1: aux2 = L_dest[i] & 0x80; break; /* Saves the msb */
//		case 2: aux2 = L_dest[1] & 0xc0; break; /* Saves the 2 msb */
//		case 3: aux2 = L_dest[1] & 0xe0; break; /* Saves the 3 msb */
//		case 4: aux2 = L_dest[1] & 0xf0; break; /* Saves the 4 msb */
//		case 5: aux2 = L_dest[1] & 0xf8; break; /* Saves the 5 msb */
//		case 6: aux2 = L_dest[1] & 0xfc; break; /* Saves the 6 msb */
//		case 7: aux2 = L_dest[1] & 0xfe; break; /* Saves the 7 msb */
//		case 8: aux2 = L_dest[1] & 0xff; break; /* Saves the 8 msb */
//		default:
//			printf("ERROR: Function shift_left the parameter position is not on the limits\n");
//			printf("Shift left %d positions\n",pos);
//			exit();
//			break;
//
//		}
//
//		/* Shift left x positions */
//		L_dest[i] <<= pos ;
//		/* The msb aux in this moment will be the lsb */
//
//		aux = aux >> (8-pos);
//		L_dest[i] = L_dest[i] | aux ;
//		/* Save the msb of the  current position string in aux,
//		 * because aux2 will receive the msb of the next position
//		 * of the string. */
//		aux = aux2 ;
//	}
//}



//void double_function(unsigned char* L_init, int length,unsigned char* L_dest){

//	/* temp - saves the msb of L[0] */
//	unsigned char temp;
//	unsigned char aux , aux2 ;
//	aux = 0x00 ;
//	//unsigned char L_dest[length] ;
//
//
//	/* Shift all the bits of the string to the left one bit*/
//
//
////	/* The last position of the array (length-1)
////	 * aux saves the most significant bit of the last
////	 * position if the array  */
//
//	for(int i = length -1 ; i >= 0 ; i--){
//
//		/* Copy the string L_init to string L_dest */
//		L_dest[i] = L_init[i];
//
//		if(i == 0){
//			/* temp - saves the msb of L[0] */
//			temp =  L_dest[0] & 0x80 ;
//		}
//
//		/* Saves the msb of the current position of string */
//		aux2 = L_dest[i] & 0x80;
//		/* Shift left of one position */
//		L_dest[i] <<= 1 ;
//		/* The msb aux in this moment will be the lsb */
//		aux >>= 7 ;
//		L_dest[i] = L_dest[i] | aux ;
//		/* Save the msb of the  current position string in aux,
//		 * because aux2 will receive the msb of the next position
//		 * of the string. */
//		aux = aux2 ;
//	}
//
//	//printf("OUTPUT: ");
//	//print_hexa_string(L_dest,4);
//
//	if(temp != 0){
//		/* The last position of the string xor 0x87 */
//		L_dest[length -1] ^= 0x87 ;
//	}
//
//	//print_hexa_string(&L_dest[length-1],1);
//	//return L_dest ;
//}


