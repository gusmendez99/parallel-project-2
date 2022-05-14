/*
 ============================================================================
 Author(s)    : G. Barlas, Gus Mendez, Roberto Figueroa, Michele Benvenuto
 
 Compile      : g++ -fpermissive -o example example.cpp 
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/des_crypt.h>

#define BUFFSIZE 1024
#define PLAINTEXT_FILEPATH "./input/plain.txt"
#define KEY_FILEPATH "./input/key.txt"

void decrypt(long key, char *ciph, int len){
    //set parity of key and do decrypt
    int result;
    long k = 0;
    for(int i=0; i<8; ++i){
        key <<= 1;
        k += (key & (0xFE << i*8));
    }
    des_setparity((char *)&k);  //el poder del casteo y &
    result = ecb_crypt((char *)&k, (char *) ciph, len, DES_DECRYPT);
    if(DES_FAILED(result) || strcmp(ciph, "") == 0) {
        if(strcmp(ciph, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Decryption Error ***\n");
        }
    }
}

void encrypt(long key, char *ciph, int len){
    //set parity of key and do decrypt
    int result;
    long k = 0;
    for(int i=0; i<8; ++i){
        key <<= 1;
        k += (key & (0xFE << i*8));
    }
    des_setparity((char *)&k);  //el poder del casteo y &
    result = ecb_crypt((char *)&k, (char *) ciph, len, DES_ENCRYPT);
    if (DES_FAILED(result) || strcmp(ciph, "") == 0) {
        if(strcmp(ciph, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Encryption Error ***\n");
        }
    } 
}

int main(void) {
    char *key = NULL;
    char *plaintext = NULL;
    size_t size = 0;
    int buffsize, orig_buffsize;
    
    /* Open plain txt in read-only mode */
    FILE *fp = fopen(PLAINTEXT_FILEPATH, "r");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    plaintext = malloc((size + 1) * sizeof(*plaintext));
    fread(plaintext, size, 1, fp);
    plaintext[size] = '\0';
    printf("Plain: %s\n", plaintext);
    
    size = 0;
    
    /* Open key txt in read-only mode */
    fp = fopen(KEY_FILEPATH, "r");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    key = malloc((size + 1) * sizeof(*key));
    fread(key, size, 1, fp);
    key[size] = '\0';
    printf("Key: %s\n", key);

    /* Encrypt plaintext, result is in encbuff */
    buffsize, orig_buffsize = strlen(plaintext);
    if(orig_buffsize % 8 >0)
        buffsize = ((orig_buffsize / 8)+1)*8;
    printf("buffsize is %d\n",buffsize);
    
    encrypt(key, (char *)plaintext, buffsize);
    printf("Encrypted: %s\n", plaintext);
    

    printf("{");
    for(int i=0; i< buffsize;i++)
        printf("%d, ", (int)plaintext[i]);
    printf("}\n");

    /* Decrypt encbuff, result is in decbuff */
    buffsize = strlen(plaintext);
    printf("buffsize is %d\n",buffsize);
    
    decrypt(key, (char *)plaintext, buffsize);
    printf("Decrypted: %s\n",plaintext);

    return 0;
}
