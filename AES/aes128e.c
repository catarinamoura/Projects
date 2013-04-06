/*
 ============================================================================
 Name     		   : Catarina Moura
 Student Number    : 340964
 Description 	   : Advanced Encryption Standard
 ============================================================================
 */



#include <stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include "aes128e.h"

/* Multiplication by two in GF(2^8). Multiplication by three is xtime(a) ^ a */
#define xtime(a) ( ((a) & 0x80) ? (((a) << 1) ^ 0x1b) : ((a) << 1) )

/* Plain Text(state) always 128 bits(4*4=16 bytes) number columns 4 bytes	*/
#define Nb 4

/* The number of rounds */
int Nr=10 ;

/* The number of 32 bit words in the key. 128(16 bytes) /32 = 4 */
int Nk=4;

/* the cipher key size, in this case 16 bytes*/
	enum keySize key_size = SIZE_16;

/* the round keySize (with 16 bytes, the lenght will be 176) */
	int roundKeySize ;

/* The array that stores the round keys values
 * RoundKey[roundKeySize]*/
unsigned char RoundKey[176];

/* Variable to indicate the position of RoundKey (AddRoundKey)  */
int RoundKey_pos = 0 ;
/* The S-box table */
static const unsigned char sbox[256] = {
//	  0	   1	 2	   3	 4	   5	 6	   7	 8	   9	 A	   B     C	   D     E	   F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,//0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,//1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,//2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,//3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,//4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,//5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,//6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,//7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,//8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,//9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,//A
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,//B
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,//C
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,//D
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,//E
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 //F
};


/* The round constant table (needed in KeyExpansion) */
static const unsigned char rcon[10] = {
		0x01, 0x02, 0x04, 0x08, 0x10,
		0x20, 0x40, 0x80, 0x1b, 0x36 };


/*
 *********************************************************************************
 ====================================	AES128e	==================================
 *********************************************************************************
 */
/* Under the 16-byte key at k, encrypt the 16-byte plaintext at p and store it at c. */
void aes128e(unsigned char *c, const unsigned char *p, const unsigned char *k) {
	int i,j,round=0;

	roundKeySize = key_size*(Nr+1); /* (in this case 176 bytes) */

	/* Copy the plain text to c */
	for(i =0 ; i<16; i++){
		c[i] = p[i];
	}

	/* The KeyExpansion routine must be called before encryption */

	 memset( RoundKey, 0, sizeof( RoundKey)); /*  Clean the RoundKey array  */
	 KeyExpansion(k);

	// print_expansion();

	 /*  Add the First round key to the state before starting the rounds. */

	AddRoundKey(c,0);
	//print_c(c);
	/* There will be Nr rounds.
	   The first Nr-1 rounds are identical.
	   These Nr-1 rounds are executed in the loop below. */

	for(round=1;round<Nr;round++){
	//	if(round == 1){puts("StartRound");print_c(c);}
		SubBytes(c);//if(round == 1){puts("Subbytes");print_c(c);}
		ShiftRows(c);//if(round == 1){puts("ShiftRows");print_c(c);}
		MixColumns(c);//if(round == 1){puts("MixColumns");print_c(c);}
		AddRoundKey(c,round);//if(round == 1){puts("Add Round Key");print_c(c);}
	}

	/* The last round is given below.The MixColumns function is not here in the last round. */
	SubBytes(c);
	ShiftRows(c);
	AddRoundKey(c,Nr);
}


/*
*********************************************************************************
=================================	Key Expansion	=============================
*********************************************************************************
*/


/* Rotate the word eight bits to the left
 *
 * e.g rotate(1d2c3a4f) = 2c3a4f1d
 *
 * word is an char array of size 4 (32 bit)
 */
void rotate(unsigned char *word)
{
    unsigned char c;
    int i;

    c = word[0];
    for (i = 0; i < 3; i++)
        word[i] = word[i+1];
    word[3] = c;
}

void core(unsigned char *word, int iteration)
{
    int i;

    /* rotate the 32-bit word 8 bits to the left */
    rotate(word);

    /* apply S-Box substitution on all 4 parts of the 32-bit word */
    for (i = 0; i < 4; ++i)
    {
        word[i] = sbox[word[i]];
    }

    /* XOR the output of the rcon operation with i to the first part (leftmost) only */
    word[0] = word[0]^rcon[iteration];
}

void KeyExpansion(const unsigned char *key){

    /* current round keySize, in bytes */
    int currentSize = 0;
    int rconIteration = 0;
    int i;
    unsigned char t[4] = {0};   // temporary 4-byte variable

    /* set the 16 bytes of the round key to the input key */
    for (i = 0; i < key_size; i++)
        RoundKey[i] = key[i];
    currentSize += key_size;

    while (currentSize < roundKeySize)
    {
        /* assign the previous 4 bytes to the temporary value t */
        for (i = 0; i < 4; i++)
        {
            t[i] = RoundKey[(currentSize - 4) + i];
        }

        /* every 16 we apply the core schedule to t and increment rconIteration afterwards */
        if(currentSize % key_size == 0)
        {
        	core(t, rconIteration++);
        }

        /*  XOR t with the four-byte block 16 bytes before the new round key.
         * This becomes the next four bytes in the round key.
         */
        for(i = 0; i < 4; i++) {
            RoundKey[currentSize] = RoundKey[currentSize - key_size] ^ t[i];
            currentSize++;
        }
    }
}


/*
 *********************************************************************************
 =================================	SubBytes	==================================
 *********************************************************************************
 */

/* The SubBytes Function Substitutes the values in the plain text
   matrix with values in a S-box */

void SubBytes(unsigned char *c){

	int i ;

	//int lenght =sizeof(unsigned char)*strlen((const char*)c);
	//printf("LENGHT: %d\n",lenght);

	for(i=0 ; i < 16  ; i++){

		/* Get the first 4 bits(first hexadecimal letter, most significant bits) */
		unsigned char hex_1 = extractBitRange(c[i],4,4);
		/* Get the last 4 bits(second hexadecimal letter,less significant bits)	*/
		unsigned char hex_2 =extractBitRange(c[i],0,4);
	//	printf("hex1 %x\n",hex_1);
	//	printf("hex2 %x\n",hex_2);
		/* Transform the bits to integer */
		int row = (int) hex_1;
		int column = (int) hex_2;

		/* Formula to get the position of the Matrix
		 * 	16 because the matrix is 16*16 = 256 */
		int get_pos = row* 16 + column ;
		/* Store the S-box values in c */
		c[i]=sbox[get_pos];
//		printf("c[%d]= %x\n",i,c[i]);
	}
}

/* Extract a number of bits from a byte, range - how many bit you want for a byte */
unsigned char extractBitRange(char byte,int startingPos,int range){
   if(!( (startingPos >= 0 && range >= 0) && (startingPos < 8 && range <= 8) && (startingPos + range) <= 8 )){
	   printf("Not possible to extractBitRange, please revise your parameters:\n");
	   printf("Starting Position:%d\n Range:%d\n",startingPos,range);
	   return -1;
   }
   return( byte >> startingPos )&~(0xff<<(range));
}

/* Visualize the positions of the matrix */
void matrix_visio(){
	int i,j, counter = 0 ;

	for(i=0 ;i < 16 ; i++){
		puts(" ");
		for(j=0 ; j< 16 ; j++ ){
			printf("%03d\t",counter);
			counter++;
		}	}
	puts("");
}

/* To test if the formula to get the position on the matrix is right */
void formula_test(){
	int i,j ;
	for(i=0 ;i < 16 ; i++){
		puts(" ");
		for(j=0 ; j< 16 ; j++ ){
			printf("%03d\t",i* 16 + j);
		}	}
	puts("");
}

/*
 *********************************************************************************
 =================================	ShiftRows	==================================
 *********************************************************************************
 */

/*	The ShiftRows() function shifts the rows in the state to the left.
 *	Each row is shifted with different offset.
 *	Offset = Row number. So the first row is not shifted.
 *	The second shift once , the third shift two times etc .. */

void ShiftRows(unsigned char *c){

	unsigned char temp ;
/* Matrix [0 4 8 12(row1) ; 1 5 9 13(row2); 2 6 10 11(row3); 3 7 11 15(row4)]*/

	/* Row 0 - Doesn't suffer any rotation */

	/* Row 1 - Shift one row to the left  */

	temp = c[1] ; /*  Row 1 column 0*/

	c[1] = c[5];
	c[5] = c[9];
	c[9] = c[13];
	c[13] = temp ;

	/* Row 2 - Shift two rows to the left  */

	temp = c[2]; /*  Row 2 column 0*/
	c[2]= c[10];
	c[10] = temp;

	temp = c[6];
	c[6] = c[14];
	c[14] = temp;

	/* Row 3 - Shift three rows to the left  */

	temp = c[3] ; /*  Row 3 column 0*/
	c[3] = c[15];
	c[15] = c[11];
	c[11] = c[7];
	c[7] = temp;
}

/*
 *********************************************************************************
 =================================	MixColumns	==================================
 *********************************************************************************
 */

/* MixColumns function is a transformation on c that takes all columns of the c
 *  and mixes the data to produce new columns  */
void MixColumns(unsigned char *c){

	int i ;
	unsigned char Tmp,Tm,t;

		for(i=0;i<4;i++){
			t= c[0+i*4];
			/* c=0,4,8,12 rows 0,1,2,3 */
			Tmp = c[0+i*4] ^ c[1+i*4] ^ c[2+i*4] ^ c[3+i*4];

			Tm = c[0+i*4] ^ c[1+i*4]; Tm = xtime(Tm); c[0+i*4] ^= Tm ^Tmp ;
			Tm = c[1+i*4] ^ c[2+i*4]; Tm = xtime(Tm); c[1+i*4] ^= Tm ^Tmp ;
			Tm = c[2+i*4] ^ c[3+i*4]; Tm = xtime(Tm); c[2+i*4] ^= Tm ^Tmp ;
			Tm = c[3+i*4] ^ t; Tm = xtime(Tm); c[3+i*4] ^= Tm ^Tmp ;
		}
}

/*
 *********************************************************************************
 =================================	AddRoundKey	==================================
 *********************************************************************************
 */

/*  This function adds the round key to c.
 *  The round key is added to the c by an XOR function. */

void AddRoundKey(unsigned char *c,int round){

	int i,j;
	RoundKey_pos = round*16;

	for(i=0; i<16 ; i++){
	c[i] = c[i]^RoundKey[i+RoundKey_pos];
	}
}

/*
 *********************************************************************************
 =================================	Print 	======================================
 *********************************************************************************
 */

void print_c(unsigned char *c){

	printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[0],c[4],c[8],c[12]);
	printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[1],c[5],c[9],c[13]);
	printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[2],c[6],c[10],c[14]);
	printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[3],c[7],c[11],c[15]);
	printf("\n\n");
}

//void print_c(unsigned char *c){
//	int i,j ;
//		for(i =0 ; i<16; i++){
//			if(i == 4 || i==8 || i==12){
//				puts(" ");
//			}
//			printf("0x%x\t",c[i]);
//				}
//		printf("\n\n");
//}

void print_expansion(){

	unsigned char *c = RoundKey ;

	int j0=0,j1=1,j2=2,j3=3,j4=4,j5=5,j6=6,j7=7,j8=8,j9=9;
		int j10=10,j11=11,j12=12,j13=13,j14=14,j15=15 ;

	for(int i=0; i< 176 ; i += 16){

			printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[j0],c[j4],c[j8],c[j12]);
			printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[j1],c[j5],c[j9],c[j13]);
			printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[j2],c[j6],c[j10],c[j14]);
			printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",c[j3],c[j7],c[j11],c[j15]);
			printf("\n\n");

			j0+=16;j1+=16;j2+=16;j3+=16;j4+=16;j5+=16;j6+=16;j7+=16;j8+=16;j9+=16;
			j10+=16;j11+=16;j12+=16;j13+=16;j14+=16;j15+=16;
	}
}

//void print_expansion(){
//	int i,j =4, k=16 , l=2 ;
//	puts("oi");
//	for(i = 0 ; i<240; i++){
//
//			if(i == j){
//				puts(" ");
//				j = j+4;
//			}
//				if( (j-4) == k){
//					printf("%d\t  \n",l);
//					l++ ;
//					k += 16 ;
//				}
//			printf("0x%x\t",RoundKey[i]);
//				}
//
//		printf("\n\n");
//}

