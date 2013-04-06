/* Implement the following API.
 * You can add your own functions, but don't modify below this line.
 */

/* Hash the message at m and store the 32-byte hash at h. The length of m in bytes is given at n. */
void groestl256(unsigned char *h, const unsigned char *m, unsigned long long n);
void AddRoundConstant(unsigned char *A,int flag,int round);
void SubBytes(unsigned char *A);
unsigned char extractBitRange(char byte,int startingPos,int range);

void ShiftBytes(unsigned char *A,int flag);

void MixBytes(unsigned char *A);
void circulant(unsigned char *B, unsigned char *B_row,int number_rows,int number_columns);
unsigned char B_transformation(unsigned char B, unsigned char A);


void print_matrix(unsigned char *m);
void view_matrix();
void print_matrix_inv(unsigned char *m);
void print_matrix_32(unsigned char *m);



void compare_matrix(unsigned char* string_result,unsigned char* string_test,int length,char* text);
void test_P(unsigned char *P_program);
void test_Q(unsigned char *Q_program);
void test_AddRoundConstantP(unsigned char *P_program);
void test_SubBytesP(unsigned char *P_program);
void test_ShiftBytes(unsigned char *A_program,int flag);
void test_B(unsigned char *B_program);
void test_MixBytes(unsigned char *A_program,int flag);


/*
	0	  8	 16	 24	 32	 40	 48	 56
	1	  9	 17	 25	 33	 41	 49	 57
	2	 10	 18	 26	 34	 42	 50	 58
	3	 11	 19	 27	 35	 43	 51	 59
	4	 12	 20	 28	 36	 44	 52	 60
	5	 13	 21	 29	 37	 45	 53	 61
	6	 14	 22	 30	 38	 46	 54	 62
	7	 15	 23	 31	 39	 47	 55	 63
*/














//void B_aux(unsigned char B, unsigned char A){
//
//	int flag = 0; /* To certify A is computed */
//
//	if( B == 0x02){
//		/* 2 hex is 10 bin is x
//		 * So is xtime(A[i]) */
//
//			A = xtime(A);
//			flag=1 ;
//		}
//
//		if( B == 0x03){
//			/* 3 hex is 11 bin is x +1
//			 * So is xtime(A[i]) xor A[i] */
//
//			A[i] = xtime(A[i]) ^ A[i];
//			flag=1 ;
//		}
//
//		if( B[i] == 0x04){
//			/* 4 hex is 100 bin is x^2
//			 * So is xtime(xtime(A[i])) */
//
//			A[i] = xtime(xtime(A[i])) ;
//			flag=1 ;
//		}
//		if( B[i] == 0x05){
//			/* 5 hex is 101 bin is x^2 +1
//			 * So is xtime(xtime(A[i])) xor A[i] */
//
//			A[i] = xtime(xtime(A[i])) ^ A[i];
//			flag=1 ;
//		}
//		if( B[i] == 0x07){
//			/* 7 hex is 111 bin is x^2+x+1
//			 * So is xtime(xtime(A[i])) xor xtime(A[i]) xor A[i] */
//
//			A[i] = xtime(xtime(A[i])) ^ xtime(A[i]) ^ A[i];
//			flag=1 ;
//		}
//
//		if (flag == 0){
//			printf("Error: MixBytes function not implemented\n");
//			printf("Provide a implementation please\n");
//		}
//
//
//
