/* Implement the following API.
 * You can add your own functions, but don't modify below this line.
 */


/* Under the 16-byte (128-bit) key at k and the 12-byte (96-bit) nonce at n, encrypt the plaintext at p and store it at c.
   The length of the plaintext is a multiple of 16 bytes given at len (e.g., len = 2 for a 32-byte p). The length of the
   ciphertext c is (len+1)*16 bytes. */

void aes128ocb(unsigned char *c, const unsigned char *k, const unsigned char *n, const unsigned char *p, const unsigned int len);
void zeros(unsigned char* zero_string,int bit_start_pos,int number_of_bits);

void double_function(unsigned char* L_init,unsigned char* L_dest,int length);
void shift_left(unsigned char* L_init,unsigned char* L_dest,int length_byte,int start_pos_bit,int equa_len);

void nonce_function(const unsigned char* nonce,int length,unsigned char* Nonce);
void Ktop_function(unsigned char* Nonce,unsigned char* Ktop,int length,const unsigned char *key);
void Stretch_function(unsigned char* Ktop,int length_Ktop,unsigned char* Stretch);


/* Test functions */

void compare_hex_string(unsigned char* string1,unsigned char* string2,int length,char* text);
void test_lstar(unsigned char* L_star);
void test_ldollar(unsigned char* L_dollar);
void test_lm(unsigned char* L_m,int m);
void test_Ktop(unsigned char* Ktop);
void test_Stretch(unsigned char* Stretch);
void test_Offset(unsigned char* Offset,int pos);
void test_Checksum(unsigned char* Checksum,int pos);
void test_Cipher(unsigned char* Cipher,int pos);
void test_Tag(unsigned char* Tag);


void test_shiftL();

/* Print functions */

void print_bit_string(unsigned char* string,int length);
void print_hexa_string(unsigned char* string,int length);
void print_hexa_string_continuous(unsigned char* string,int length);


//
//void shift_left(unsigned char* L_init,unsigned char* L_dest,int length, int pos){
//
//	/* The string_position will give the position of the string to
//	 * start doing the shift. Dividing by 8 because is 8 bits
//	 * (1 byte = char). Eg: If you want to shift 18 positions
//	 * you will start on position number two of the string */
//	int one_byte = 8 ;
//	int string_position = pos/one_byte ;
//	int msbits_of_byte = pos%one_byte;
//	int lsbits_of_byte = one_byte - (pos%one_byte);
//
//	/* Shift left x bits on the string */
//	for(int i = 0; i<length; i++){
//
//		/* Stopping Condition
//		 *  When you do a Shift_left, the last bits
//		 *  of the array will be fill by zeros. */
//		if ( i+ string_position == (length-1)   ){
//
//			if(pos%8 != 0){
//				L_dest[i] =  L_init[i+string_position] << msbits_of_byte ;
//
//				if(string_position != 0){
//					/* Starting in i+1 L_dest will be put with zeros */
//					zeros(L_dest,i+1,string_position);
//				}
//
//			}else{
//				/* Will fill L_dest with zeros starting in i */
//				zeros(L_dest,i,string_position);
//			}
//			return ;
//		}
//
//		L_dest[i] = L_init[i+string_position] <<   msbits_of_byte |
//					L_init[i+string_position+1] >> lsbits_of_byte ;
//	}
//}




