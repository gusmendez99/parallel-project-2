/*
    ============================================================================
    Name        : test-c.c
    Description : Testing Project
                  Trying to do a C version of this perl code:
                    my $cipher = Crypt::CBC->new( -key => $salt_key, -cipher => 'DES' -header => 'none');
                    my $enc_text = $cipher->encrypt_hex($text);
    Requires    : -lcrypt
    References  :
      Function: cbc_crypt (char *key, char *blocks, unsigned len, unsigned mode, char *ivec)
      GNU C Library: DES Encryption (http://www.gnu.org/software/libc/manual/html_node/DES-Encryption.html#DES-Encryption)
      cbc_crypt (http://unix.derkeiler.com/Newsgroups/comp.unix.programmer/2012-10/msg00023.html)
    ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/des_crypt.h>

#define BUFFSIZE 420
int main(void) {
    //  char key[]     = "aBcDeFg1";
    char key[9];
    sprintf(key,"aBcDeFg1");
    //char pass[]    = "mypass1234test";
    char pass[BUFFSIZE];
    sprintf(pass,"mypass1234test");

    //  char encbuff[] = "87654321";
    char ivec[9];
    sprintf(ivec,"87654321");
    //  char decbuff[] = "87645321";
    char ivectemp[9];
    strcpy(ivectemp,ivec);
    int buffsize;
    int result;

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
    result = cbc_crypt(key, pass, buffsize, DES_ENCRYPT | DES_SW, ivectemp);
    if (DES_FAILED(result) || strcmp(pass, "") == 0) {
        if(strcmp(pass, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Encryption Error ***\n");
        }
    } else {
        printf("%s\n", pass);
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
    strcpy(ivectemp,ivec);
    result = cbc_crypt(key, pass, buffsize, DES_DECRYPT | DES_SW, ivectemp);
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