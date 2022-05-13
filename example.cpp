/*
 ============================================================================
 Author(s)    : G. Barlas, Gus Mendez, Roberto Figueroa, Michele Benvenuto
 
 Compile      : g++ -o example example.cpp 
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/des_crypt.h>

#define BUFFSIZE 1024
#define PLAINTEXT_FILEPATH "./input/plain.txt"
#define KEY_FILEPATH "./input/key.txt"

int main(void) {
    char *key = NULL;
    char *pass = NULL;
    size_t size = 0;
    int buffsize, result;
    
    /* Open plain txt in read-only mode */
    FILE *fp = fopen(PLAINTEXT_FILEPATH, "r");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    pass = malloc((size + 1) * sizeof(*pass));
    fread(pass, size, 1, fp);
    pass[size] = '\0';
    printf("Plain: %s\n", pass);
    
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

    des_setparity(key);

    /* Encrypt pass, result is in encbuff */
    buffsize = strlen(pass);
    printf("buffsize is %d\n",buffsize);
    /* Add to pass to ensure size is divisable by 8. */
    while (buffsize % 8 && buffsize<BUFFSIZE) {
        pass[buffsize++] = '\0';
    }
    printf("pass is %s\n",pass);
    printf("buffsize is %d\n",buffsize);
    printf("Encrypted: ");
    result = ecb_crypt(key, pass, buffsize, DES_ENCRYPT | DES_SW);
    if (DES_FAILED(result) || strcmp(pass, "") == 0) {
        if(strcmp(pass, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Encryption Error ***\n");
        }
    } else {
        printf("%s\n", pass);
        printf("%X\n", pass);
	printf("%x\n", pass);
    }

    /* Decrypt encbuff, result is in decbuff */

    /* FIXME: Decryption doesn't work:
            Encrypted: ,ï¿½7&ï¿½ï¿½ï¿½8
            Decrypted: *** Decryption Error ***
     */
    buffsize = strlen(pass);
    printf("buffsize is %d\n",buffsize);
    /* Add to pass to ensure size is divisable by 8. */
    while (buffsize % 8 && buffsize<BUFFSIZE) {
        pass[buffsize++] = '\0';
    }
    printf("buffsize is %d\n",buffsize);
    printf("Decrypted: ");
    //keeping the same initialization vector for decrypting, encrypt has altered ivectemp
    
    result = ecb_crypt(key, pass, buffsize, DES_DECRYPT | DES_SW);
    if(DES_FAILED(result) || strcmp(pass, "") == 0) {
        if(strcmp(pass, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Decryption Error ***\n");
        }
    } else {
        printf("%s\n",pass);
    }

    return 0;
}
