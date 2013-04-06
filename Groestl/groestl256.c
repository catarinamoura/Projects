/*
 ============================================================================
 Name     		   : Catarina Moura
 Student Number    : 340964
 Description 	   : Groestel Hash Function
 Note: Don't forget how the matrix is constructed to visualize use function view_matrix()
 ============================================================================
 */



#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "groestl256.h"

#define NUMBYTES 64

/* Number of rounds to the permutations
 * It will be 10 rounds to digest sizes 8-256
 * 14 rounds to digest sizes 264-512 */
#define ROUND 10

/* Multiplication by two in GF(2^8). Multiplication by three is xtime(a) ^ a */
#define xtime(a) ( ((a) & 0x80) ? (((a) << 1) ^ 0x1b) : ((a) << 1) )

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


/* Matrix P 8*8 */

static const unsigned char P_512[64] = {

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Matrix Q 8*8 */

static const unsigned char Q_512[64] = {

		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaf,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f};

static void groestl_pad(unsigned char **m_pad, unsigned long long *len_pad, const unsigned char *m, const unsigned long long n);

/* Hash the message at m and store the 32-byte hash at h. The length of m in bytes is given at n. */


/*
 *********************************************************************************
 ================================	Groestel256		==============================
 *********************************************************************************
 */

void groestl256(unsigned char *h, const unsigned char *m, unsigned long long n) {

	unsigned char *m_pad;					/* An array for the padded message */
	unsigned long long len_pad;			/* The length of the padded message (in 64-byte blocks) */
	unsigned char P[64];
	unsigned char Q[64];
	int i,j ;

	//view_matrix();

	groestl_pad(&m_pad,&len_pad,m,n);	/* Perform the message padding */
	unsigned char hi[len_pad+1][64];

	/* Initial state
	 * Goesteln = Goestel256
	 * ivn = iv256 = [000...0100]Hex
	 * 01 will stay 63 position
	 * 00 will go to 64 position */
	for(i=0; i< NUMBYTES; i++){

		hi[0][i] = 0x00;
		if(i == 62){
			hi[0][i] = 0x01;
		}
	}

	//printf("Block of contents(m_pad):\n");


	/* Compression function cosntruction
	 *  f(h,m) = P(h^m)^Q(m)^h
	 *  Begin P and Q*/

	for(i=0; i< NUMBYTES ; i++){
		P[i] = m_pad[i] ^ hi[0][i];
		Q[i] = m_pad[i];
	}

	//test_P(P);
	//test_Q(Q);

	//printf("Lenght  %llu\n",n);
	//printf("Lenght pad %llu\n",len_pad);

	/* The len_pad gives how many messages blocks we have
	 * m_pad have all the messages m1,...,mt */
	for(j =0; j<len_pad; j++){

		for(i=0; i< NUMBYTES ; i++){
			P[i] = m_pad[j*NUMBYTES+i] ^ hi[j][i];
			Q[i] = m_pad[j*NUMBYTES+i];
		}

		for(i=0; i<ROUND; i++){

			AddRoundConstant(P,0,i);
			SubBytes(P);
			ShiftBytes(P,0);
			MixBytes(P);

			AddRoundConstant(Q,1,i);
			SubBytes(Q);
			ShiftBytes(Q,1);
			MixBytes(Q);
		}

		for(i=0; i<NUMBYTES; i++){
			hi[j+1][i] = P[i] ^ Q[i] ^ hi[j][i];
		}
	}

	/* Output transformation  O(ht)= trunc_n(P(ht) xor ht)
	 * Note ht is the last hi
	 *  trunc n - truncates the output by returning only the last n bits
	 *  in this case the last 256 bits (32 bytes)*/

	for(i=0; i< NUMBYTES ; i++){
		P[i] = hi[len_pad][i];
	}

	for(i=0; i<ROUND; i++){

		AddRoundConstant(P,0,i);
		SubBytes(P);
		ShiftBytes(P,0);
		MixBytes(P);
	}

	for(i = (NUMBYTES/2); i<NUMBYTES; i++){

		h[i-(NUMBYTES/2)] = P[i] ^ hi[len_pad][i]  ;
	}

	free(m_pad); 								/* Should be at the end. */
}


/*
 *********************************************************************************
 ================================	Groestel Pad	==============================
 *********************************************************************************
 */
/* Performs the message padding. The original message and its length (in bytes) are given at
	m and n, respectively. Reserves memory for the	padded message and stores it at m_pad. 
	The length (in 512-bit blocks) is stored at len_pad. */
static void groestl_pad(unsigned char **m_pad, unsigned long long *len_pad, const unsigned char *m, const unsigned long long n) {

	/* Compute the length of the padded message (in 64-byte blocks) */
	*len_pad = (n*8 + 65 + (-n*8 - 65) % 512) / 512;	

	/* Allocate memory for the padded message */
	*m_pad = (unsigned char*) calloc(*len_pad*NUMBYTES, sizeof(unsigned char));

	/* Copy m to m_pad */
	memcpy(*m_pad, m, n);

	/* Add a bit to the end of the original message */
	(*m_pad)[n] = 0x80;

	/* Add the 64-bit representation of ((N+w+65)/512) in the end of m_pad */
	(*m_pad)[*len_pad*NUMBYTES-8] = (unsigned char) (*len_pad >> 56);
	(*m_pad)[*len_pad*NUMBYTES-7] = (unsigned char) (*len_pad >> 48);
	(*m_pad)[*len_pad*NUMBYTES-6] = (unsigned char) (*len_pad >> 40);
	(*m_pad)[*len_pad*NUMBYTES-5] = (unsigned char) (*len_pad >> 32);
	(*m_pad)[*len_pad*NUMBYTES-4] = (unsigned char) (*len_pad >> 24);
	(*m_pad)[*len_pad*NUMBYTES-3] = (unsigned char) (*len_pad >> 16);
	(*m_pad)[*len_pad*NUMBYTES-2] = (unsigned char) (*len_pad >> 8);
	(*m_pad)[*len_pad*NUMBYTES-1] = (unsigned char) (*len_pad);

}

/*
 *********************************************************************************
 ==============================	AddRoundConstant	==============================
 *********************************************************************************
 */

/* Flag 0 is P
 * Flag 1 is Q
 */
void AddRoundConstant(unsigned char *A,int flag,int round) {

	unsigned char C[64];
	int i;


	if(flag == 0){
		/* We are in P */

		for(i=0; i< NUMBYTES ; i++){
			C[i]= P_512[i] ;
		}

		for(i=0; i<NUMBYTES; i=i+8){
			C[i]= C[i] ^ round;
		}
	}

	if(flag == 1){
		/* We are in Q */
		for(i=0; i< NUMBYTES ; i++){
			C[i]= Q_512[i] ;
		}

		for(i=7; i<NUMBYTES; i=i+8){
			C[i]= C[i] ^ round;
		}
	}

	for(i=0; i<NUMBYTES;i++){

		A[i] = A[i] ^ C[i];
	}
}

/*
 *********************************************************************************
 ===================================	SubBytes	==============================
 *********************************************************************************
 */

void SubBytes(unsigned char *A){

	int i ;
	for(i=0 ; i < NUMBYTES  ; i++){

		/* Get the first 4 bits(first hexadecimal letter, most significant bits) */
		unsigned char hex_1 = extractBitRange(A[i],4,4);
		/* Get the last 4 bits(second hexadecimal letter,less significant bits)	*/
		unsigned char hex_2 =extractBitRange(A[i],0,4);

		/* Transform the bits to integer */
		int row = (int) hex_1;
		int column = (int) hex_2;

		/* Formula to get the position of the Matrix
		 * 	16 because the matrix is 16*16 = 256 */
		int get_pos = row* 16 + column ;
		/* Store the S-box values in A */
		A[i]=sbox[get_pos];

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


/*
 *********************************************************************************
 ===================================	ShiftBytes	==============================
 *********************************************************************************
 */

void ShiftBytes(unsigned char *A,int flag){

	unsigned char temp ;
	unsigned char temp2 ;
	unsigned char temp3 ;
	int i;
	unsigned char aux[64];

	if(flag == 0){
		/* It will shift the bytes of P */


		/* Row 0 - Doesn't suffer any rotation */

		/* Row 1 - Shift one row to the left  */

		temp = A[1] ; /*  Row 1 column 0*/

		A[1] = A[9];
		A[9] = A[17];
		A[17] = A[25];
		A[25] = A[33];
		A[33] = A[41];
		A[41] = A[49];
		A[49] = A[57];
		A[57] = temp;

		/* Row 2 - Shift two rows to the left  */

		temp = A[2]; /*  Row 2 column 0*/
		temp2= A[10];

		A[2]= A[18];
		A[10] = A[26];
		A[18]= A[34];
		A[26]= A[42];
		A[34]= A[50];
		A[42]= A[58];

		A[50] = temp;
		A[58] = temp2;

		/* Row 3 - Shift three rows to the left  */

		temp = A[3]; /*  Row 3 column 0*/
		temp2= A[11];
		temp3= A[19];

		A[3] = A[27];
		A[11] = A[35];
		A[19] = A[43];
		A[27] = A[51];
		A[35] = A[59];

		A[43] = temp;
		A[51] = temp2;
		A[59] = temp3;


		/* Row 4 - Shift four rows to the left  */

		temp = A[4];

		A[4] = A[36];
		A[36] = temp;

		temp = A[12];
		A[12] = A[44];
		A[44] = temp;

		temp = A[20];
		A[20] = A[52];
		A[52] = temp;

		temp= A[28];
		A[28] = A[60];
		A[60] = temp;

		/* Row 5 - Shift five rows to the left  */

		temp = A[5];
		temp2= A[13];
		temp3= A[21];

		A[5] = A[45];
		A[13] = A[53];
		A[21] = A[61];

		A[45] = temp3;
		A[53] = A[29];
		A[61] = A[37];

		A[29] = temp;
		A[37] = temp2;

		/* Row 6 - Shift six rows to the left  */

		temp = A[6];
		temp2 = A[14];

		A[6] = A[54] ;
		A[14] = A[62] ;

		A[54] = A[38];
		A[62] = A[46];

		A[46] = A[30];
		A[38] = A[22];

		A[22] = temp;
		A[30] = temp2;

		/* Row 7 - Shift seven rows to the left  */

		temp = A[7];

		A[7] = A[63];
		A[63] = A[55];
		A[55] = A[47];
		A[47] = A[39];
		A[39] = A[31];
		A[31] = A[23];
		A[23] = A[15];
		A[15] = temp;

	}

	if(flag == 1){

		/* It will shift the bytes of Q */

		/* Row 0 - Shift one row to the left  */

		temp = A[0] ; /*  Row 0 column 0*/

		A[0] = A[8];
		A[8] = A[16];
		A[16] = A[24];
		A[24] = A[32];
		A[32] = A[40];
		A[40] = A[48];
		A[48] = A[56];
		A[56] = temp;

		/* Row 1 - Shift three rows to the left  */

		temp = A[1]; /*  Row 1 column 0*/
		temp2= A[9];
		temp3= A[17];

		A[1] = A[25];
		A[9] = A[33];
		A[17] = A[41];
		A[25] = A[49];
		A[33] = A[57];

		A[41] = temp;
		A[49] = temp2;
		A[57] = temp3;

		/* Row 2 - Shift five rows to the left  */

		temp = A[2];
		temp2= A[10];
		temp3= A[18];

		A[2] = A[42];
		A[10] = A[50];
		A[18] = A[58];

		A[42] = temp3;
		A[50] = A[26];
		A[58] = A[34];

		A[26] = temp;
		A[34] = temp2;

		/* Row 3 - Shift seven rows to the left  */

		temp = A[3];

		A[3] = A[59];
		A[59] = A[51];
		A[51] = A[43];
		A[43] = A[35];
		A[35] = A[27];
		A[27] = A[19];
		A[19] = A[11];
		A[11] = temp;

		/* Row 4 - Doesn't suffer any rotation */

		/* Row 5 - Shift two rows to the left  */


		temp = A[5]; /*  Row 5 column 0*/
		temp2= A[13];

		A[5]= A[21];
		A[13] = A[29];
		A[21]= A[37];
		A[29]= A[45];
		A[37]= A[53];
		A[45]= A[61];

		A[53] = temp;
		A[61] = temp2;

		/* Row 6 - Shift four rows to the left  */

		temp = A[6];

		A[6] = A[38];
		A[38] = temp;

		temp = A[14];
		A[14] = A[46];
		A[46] = temp;

		temp = A[22];
		A[22] = A[54];
		A[54] = temp;

		temp= A[30];
		A[30] = A[62];
		A[62] = temp;

		/* Row 7 - Shift six rows to the left  */

		temp = A[7];
		temp2 = A[15];

		A[7] = A[55] ;
		A[15] = A[63] ;

		A[55] = A[39];
		A[63] = A[47];

		A[47] = A[31];
		A[39] = A[23];

		A[23] = temp;
		A[31] = temp2;
	}
}

/*
 *********************************************************************************
 =================================	MixBytes	==================================
 *********************************************************************************
 */

void MixBytes(unsigned char *A){

	int i,j,l;
	unsigned char B[64];
	unsigned char B_row[8] = {0x02,0x02,0x03,0x04,0x05,0x03,0x05,0x07};
	unsigned char A_aux[64];
	memset( A_aux, '\0', 64); /*  Clean the A_aux array  */
	circulant(B,B_row,8,8);


	/* Don't forget how the matrix is constructed
	 * to visualize use function view_matrix() .
	 *
	 * First I multiply row B to column A
	 * Iterating the same row B and the same column A with j
	 * After  Iterating the lines B with same column A with i
	 * At Last to iterate the rows of A I used l
	 * */
	for(l=0; l<8; l++){
		for(i=0; i<8; i++){
			for(j=0; j<8; j++){
				A_aux[i+l*8] ^= B_transformation(B[i+j*8],A[j+l*8]);
			}
		}
	}

	/* Copy the values of A_aux to matrix A */
	for(i=0; i<NUMBYTES; i++){
		A[i] = A_aux[i];
	}
}


/* Apply the transformation */
unsigned char B_transformation(unsigned char B, unsigned char A){

	if( B == 0x02){
		/* 2 hex is 10 bin is x
		 * So is xtime(A[i]) */

		return xtime(A);
	}

	if( B == 0x03){
		/* 3 hex is 11 bin is x +1
		 * So is xtime(A[i]) xor A[i] */

		return (xtime(A) ^ A);
	}

	if( B == 0x04){
		/* 4 hex is 100 bin is x^2
		 * So is xtime(xtime(A[i])) */

		return xtime(xtime(A)) ;
	}
	if( B == 0x05){
		/* 5 hex is 101 bin is x^2 +1
		 * So is xtime(xtime(A[i])) xor A[i] */

		return (xtime(xtime(A)) ^ A);
	}
	if( B == 0x07){
		/* 7 hex is 111 bin is x^2+x+1
		 * So is xtime(xtime(A[i])) xor xtime(A[i]) xor A[i] */

		return (xtime(xtime(A)) ^ xtime(A) ^ A);
	}

	printf("Error: MixBytes function not implemented\n");
	printf("Provide a implementation please\n");
	exit(0);
}

/* Circulant each row is equal to the row above
 * rotated right by one position.
 * B_row - Receive the row example
 * B - Store thr rotations on the matrix B
 * number_rows*length of B_row(give us the columns)
 * number_rows * number_columns will be the size of
 * the matrix B */
void circulant(unsigned char *B, unsigned char *B_row,int number_rows,int number_columns){

	for(int i=0; i< number_rows; i++){

		/* Copying the B_row to the matrix B */
		for(int j=0; j<number_columns; j++){

			B[i+j*number_columns] = B_row[j];
		}

		/* Doing the rotation of B_row in order to the
		 * next round will be copied for matrix B */
		for(int j=0; j<number_columns; j++){

			if(j == (number_columns-1) ){
				B_row[0] =B[i+j*number_columns];
			}else {
				B_row[j+1] = B[i+j*number_columns] ;
			}
		}
	}
	//test_B(B);
}


/*
 *********************************************************************************
 =================================	Print 	======================================
 *********************************************************************************
 */


void print_matrix(unsigned char *m){

	for(int i=0; i< 8; i++){

		printf("0x%02x\t 0x%02x\t 0x%02x\t 0x%02x\t 0x%02x\t 0x%02x\t 0x%02x\t 0x%02x\t\n",m[i],m[i+8],m[i+16],m[i+24],m[i+32],m[i+40],m[i+48],m[i+56]);
	}
	printf("\n\n");
}

void print_matrix_32(unsigned char *m){

	for(int i=0; i< 8; i++){

		printf("0x%02x\t 0x%02x\t 0x%02x\t 0x%02x\t \n",m[i],m[i+8],m[i+16],m[i+24]);
	}
	printf("\n\n");
}

void view_matrix(){

	for(int i=0; i< 8; i++){
		printf("%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t\n",i,i+8,i+16,i+24,i+32,i+40,i+48,i+56);
	}
}

void print_matrix_inv(unsigned char *m){

	for(int i=0;i<8;i++){

		for(int j=0;j<8;j++){

			printf("0x%02x, ",m[i*8+j]);
		}
		printf("\n");
	}
}

/*
 *********************************************************************************
 =================================	Testing 	==================================
 *********************************************************************************
 */

void compare_matrix(unsigned char* string_result,unsigned char* string_test,int length,char* text){

	for(int i =0 ; i < length; i++){

		if(string_result[i] == string_test[i]){
			continue ;
		}else{
			printf("%s: NOT EQUAL to the output\n",text);
			printf("Failed on position[%d]\n",i);
			printf("%s[%d]_program:0x%02x (16)\n",text,i,string_result[i]);
			printf("%s[%d]_test:0x%02x (16)\n",text,i,string_test[i]);
			return;
		}
	}
	printf("%s: OK\n",text);
}


void test_P(unsigned char *P_program){

	unsigned char P_test[64] = {
			0x61, 0x62, 0x63, 0x64, 0x62, 0x63, 0x64, 0x65,
			0x63, 0x64, 0x65, 0x66, 0x64, 0x65, 0x66, 0x67,
			0x65, 0x66, 0x67, 0x68, 0x66, 0x67, 0x68, 0x69,
			0x67, 0x68, 0x69, 0x6a, 0x68, 0x69, 0x6a, 0x6b,
			0x69, 0x6a, 0x6b, 0x6c, 0x6a, 0x6b, 0x6c, 0x6d,
			0x6b, 0x6c, 0x6d, 0x6e, 0x6c, 0x6d, 0x6e, 0x6f,
			0x6d, 0x6e, 0x6f, 0x70, 0x6e, 0x6f, 0x70, 0x71,
			0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};

	compare_matrix(P_program,P_test,64,"P");
}

void test_Q(unsigned char *Q_program){

	unsigned char Q_test[64] = {
			0x61, 0x62, 0x63, 0x64, 0x62, 0x63, 0x64, 0x65,
			0x63, 0x64, 0x65, 0x66, 0x64, 0x65, 0x66, 0x67,
			0x65, 0x66, 0x67, 0x68, 0x66, 0x67, 0x68, 0x69,
			0x67, 0x68, 0x69, 0x6a, 0x68, 0x69, 0x6a, 0x6b,
			0x69, 0x6a, 0x6b, 0x6c, 0x6a, 0x6b, 0x6c, 0x6d,
			0x6b, 0x6c, 0x6d, 0x6e, 0x6c, 0x6d, 0x6e, 0x6f,
			0x6d, 0x6e, 0x6f, 0x70, 0x6e, 0x6f, 0x70, 0x71,
			0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	compare_matrix(Q_program,Q_test,64,"Q");
}

void test_AddRoundConstantP(unsigned char *P_program){

	unsigned char P_test[64] = {
			0x61, 0x62, 0x63, 0x64, 0x62, 0x63, 0x64, 0x65,
			0x73, 0x64, 0x65, 0x66, 0x64, 0x65, 0x66, 0x67,
			0x45, 0x66, 0x67, 0x68, 0x66, 0x67, 0x68, 0x69,
			0x57, 0x68, 0x69, 0x6a, 0x68, 0x69, 0x6a, 0x6b,
			0x29, 0x6a, 0x6b, 0x6c, 0x6a, 0x6b, 0x6c, 0x6d,
			0x3b, 0x6c, 0x6d, 0x6e, 0x6c, 0x6d, 0x6e, 0x6f,
			0x0d, 0x6e, 0x6f, 0x70, 0x6e, 0x6f, 0x70, 0x71,
			0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};

	compare_matrix(P_program,P_test,64,"AddRoundConstant_P");
}

void test_SubBytesP(unsigned char *P_program){

	unsigned char P_test[64] = {
			0xef, 0xaa, 0xfb, 0x43, 0xaa, 0xfb, 0x43, 0x4d,
			0x8f, 0x43, 0x4d, 0x33, 0x43, 0x4d, 0x33, 0x85,
			0x6e, 0x33, 0x85, 0x45, 0x33, 0x85, 0x45, 0xf9,
			0x5b, 0x45, 0xf9, 0x02, 0x45, 0xf9, 0x02, 0x7f,
			0xa5, 0x02, 0x7f, 0x50, 0x02, 0x7f, 0x50, 0x3c,
			0xe2, 0x50, 0x3c, 0x9f, 0x50, 0x3c, 0x9f, 0xa8,
			0xd7, 0x9f, 0xa8, 0x51, 0x9f, 0xa8, 0x51, 0xa3,
			0x8c, 0x63, 0x63, 0x63, 0x63, 0x63, 0x7c, 0x63};

	compare_matrix(P_program,P_test,64,"SubBytes_P");
}

void test_ShiftBytes(unsigned char *A_program,int flag){

	if(flag == 0){
		unsigned char P_test[64] = {
				0xef, 0x43, 0x85, 0x02, 0x02, 0x3c, 0x51, 0x63,
				0x8f, 0x33, 0xf9, 0x50, 0x50, 0xa8, 0x7c, 0x4d,
				0x6e, 0x45, 0x7f, 0x9f, 0x9f, 0x63, 0x43, 0x85,
				0x5b, 0x02, 0x3c, 0x51, 0x63, 0xfb, 0x33, 0xf9,
				0xa5, 0x50, 0xa8, 0x63, 0xaa, 0x4d, 0x45, 0x7f,
				0xe2, 0x9f, 0x63, 0x43, 0x43, 0x85, 0x02, 0x3c,
				0xd7, 0x63, 0xfb, 0x33, 0x33, 0xf9, 0x50, 0xa8,
				0x8c, 0xaa, 0x4d, 0x45, 0x45, 0x7f, 0x9f, 0xa3};

		compare_matrix(A_program,P_test,64,"ShiftBytes_P");
	}

	if(flag == 1){

		unsigned char Q_test[64] = {
				0xde, 0x88, 0x4f, 0x16, 0x5e, 0x46, 0xdc, 0x28,
				0xb8, 0x2a, 0x60, 0x14, 0x14, 0x90, 0x81, 0x73,
				0x46, 0xdc, 0x16, 0xee, 0xee, 0x22, 0x73, 0xb8,
				0x90, 0x81, 0xde, 0x88, 0x88, 0x4f, 0x16, 0xc4,
				0x22, 0x16, 0xb8, 0x2a, 0x2a, 0x60, 0x14, 0x4e,
				0x4f, 0x5e, 0x46, 0xdc, 0xdc, 0x16, 0xee, 0x49,
				0xd2, 0x14, 0x90, 0x81, 0x81, 0xde, 0x88, 0xb5,
				0x0b, 0xee, 0x22, 0x73, 0x16, 0xb8, 0x2a, 0xba};

		compare_matrix(A_program,Q_test,64,"ShiftBytes_Q");
	}
}

void test_B(unsigned char *B_program){

	//	unsigned char B_test[64] = {
	//			0x02, 0x02, 0x03, 0x04, 0x05, 0x03, 0x05, 0x07,
	//			0x07, 0x02, 0x02, 0x03, 0x04, 0x05, 0x03, 0x05,
	//			0x05, 0x07, 0x02, 0x02, 0x03, 0x04, 0x05, 0x03,
	//			0x03, 0x05, 0x07, 0x02, 0x02, 0x03, 0x04, 0x05,
	//			0x05, 0x03, 0x05, 0x07, 0x02, 0x02, 0x03, 0x04,
	//			0x04, 0x05, 0x03, 0x05, 0x07, 0x02, 0x02, 0x03,
	//			0x03, 0x04, 0x05, 0x03, 0x05, 0x07, 0x02, 0x02,
	//			0x02, 0x03, 0x04, 0x05, 0x03, 0x05, 0x07, 0x02};
	unsigned char B_test[64] = {
			0x02, 0x07, 0x05, 0x03, 0x05, 0x04, 0x03, 0x02,
			0x02, 0x02, 0x07, 0x05, 0x03, 0x05, 0x04, 0x03,
			0x03, 0x02, 0x02, 0x07, 0x05, 0x03, 0x05, 0x04,
			0x04, 0x03, 0x02, 0x02, 0x07, 0x05, 0x03, 0x05,
			0x05, 0x04, 0x03, 0x02, 0x02, 0x07, 0x05, 0x03,
			0x03, 0x05, 0x04, 0x03, 0x02, 0x02, 0x07, 0x05,
			0x05, 0x03, 0x05, 0x04, 0x03, 0x02, 0x02, 0x07,
			0x07, 0x05, 0x03, 0x05, 0x04, 0x03, 0x02, 0x02};

	compare_matrix(B_program,B_test,64,"B");
}
void test_MixBytes(unsigned char *A_program,int flag){

	if(flag == 0){
		unsigned char P_test[64] = {
				0xad, 0xe9, 0xe4, 0x27, 0x0a, 0x2a, 0x46, 0x88,
				0xaf, 0x7c, 0x73, 0xc6, 0x0b, 0x21, 0x9a, 0xa9,
				0x0f, 0x03, 0x3b, 0x8b, 0x6e, 0x99, 0xef, 0xac,
				0x6d, 0xdd, 0x73, 0xdd, 0xfd, 0x9e, 0x76, 0xa1,
				0x4a, 0xb3, 0x20, 0x43, 0x06, 0x54, 0x8c, 0x7a,
				0x36, 0xc4, 0x5e, 0x8a, 0xba, 0x02, 0xd8, 0xb2,
				0x38, 0x5f, 0x6b, 0x34, 0x49, 0x5f, 0x57, 0xab,
				0xce, 0xa7, 0xe2, 0x91, 0xb9, 0x4a, 0xe8, 0x79};

		compare_matrix(A_program,P_test,64,"MixBytes_P");
	}
}



/*
 *********************************************************************************
 =================================	DEPRECATED 	==================================
 *********************************************************************************
 */

//void print_matrix(unsigned char *m){
//
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[0], m[8],m[16],m[24],m[32],m[40],m[48],m[56]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[1], m[9],m[17],m[25],m[33],m[41],m[49],m[57]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[2],m[10],m[18],m[26],m[34],m[42],m[50],m[58]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[3],m[11],m[19],m[27],m[35],m[43],m[51],m[59]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[4],m[12],m[20],m[28],m[36],m[44],m[52],m[60]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[5],m[13],m[21],m[29],m[37],m[45],m[53],m[61]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[6],m[14],m[22],m[30],m[38],m[46],m[54],m[62]);
//		printf("0x%x\t 0x%x\t 0x%x\t 0x%02x\t 0x%x\t 0x%x\t 0x%x\t 0x%02x\t\n",m[7],m[15],m[23],m[31],m[39],m[47],m[55],m[63]);
//	printf("\n\n");
//}
