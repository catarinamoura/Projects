/*
 ============================================================================
 Name     		   : Catarina Moura
 Student Number    : 340964
 Description 	   : Advanced Encryption Standard
 ============================================================================
 */
/* Implement the following API.
 * You can add your own functions, but don't modify below this line.
 */

enum keySize{
    SIZE_16 = 16,
    SIZE_24 = 24,
    SIZE_32 = 32
    };

/* Under the 16-byte key at k, encrypt the 16-byte plaintext at p and store it at c. */
void aes128e(unsigned char *c, const unsigned char *p, const unsigned char *k);
/* Key Expansion */
void rotate(unsigned char *word);
void core(unsigned char *word, int iteration);
void KeyExpansion(const unsigned char *key);

void AddRoundKey(unsigned char *c,int round);
void SubBytes(unsigned char *c);
unsigned char extractBitRange(char byte,int startingPos,int range);
void ShiftRows(unsigned char *c);
void MixColumns(unsigned char *c);

void print_c(unsigned char *c);
void print_expansion();

void matrix_visio();
void formula_test();

