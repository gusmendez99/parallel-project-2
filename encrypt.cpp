/*
 ============================================================================
 Author       : G. Barlas, Gus Mendez (support for 56-bit keys, 
                original only supports 2^31)
 
 Compile      : g++ -std=c++11 -o encrypt encrypt.cpp
 ============================================================================
*/
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <rpc/des_crypt.h>

//----------------------------------------------------------
void encrypt(long key, char *ciph, int len)
{
   // prepare key for the parity calculation. 
   // Least significant bit in all bytes should be empty
   long k = 0;
   for(int i=0;i<8;i++)
   {
     key <<= 1;
     k += (key &  (0xFE << i*8));
   }

   des_setparity((char *)&k);

   // Decrypt ciphertext
   ecb_crypt((char *)&k,(char *) ciph, 16, DES_ENCRYPT);
}

//----------------------------------------------------------
int main(int argc, char **argv)
{
  int len, origlen = strlen(argv[1]);
  if(origlen % 8 >0)
    len = ((origlen / 8)+1)*8;
  unsigned char buff[len];
  strcpy((char *)buff, argv[1]);
  long long key=std::stoll(argv[2]);

  std::cout << "Using key " << key << std::endl;

  encrypt(key, (char *)buff, len);
  std::cout<< "{";
  for(int i=0; i< len;i++)
    std::cout << (int)buff[i] << ", ";
  std::cout<< "}\n";
  return 0;
}
