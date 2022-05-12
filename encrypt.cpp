#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <rpc/des_crypt.h>

void encrypt(long key, char *ciph, int len)
{
   
   long k = 0;
   for(int i=0;i<8;i++)
   {
     key <<= 1;
     k += (key &  (0xFE << i*8));
   }

   des_setparity((char *)&k);

   ecb_crypt((char *)&k,(char *) ciph, len, DES_ENCRYPT);
}

int main(int argc, char **argv)
{
  int len, origlen = strlen(argv[1]);
  if(origlen % 8 >0)
    len = ((origlen / 8)+1)*8;
  unsigned char buff[len];
  strcpy((char *)buff, argv[1]);
  long key=atol(argv[2]);
  std::cout << "Using key " << key << std::endl;
  encrypt(key, (char *)buff, len);
  std::cout<< "{";
  for(int i=0; i< len;i++)
    std::cout << (int)buff[i] << ", ";
  std::cout<< "}\n";
  return 0;
}