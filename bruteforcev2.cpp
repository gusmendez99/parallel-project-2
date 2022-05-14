/*
 ============================================================================
 Author(s)    : G. Barlas, Gus Mendez, Roberto Figueroa, Michele Benvenuto
 
 Compile      : mpic++ -fpermissive -o bruteforcev2 bruteforcev2.cpp 
 ============================================================================
*/
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpc/des_crypt.h>
#include <mpi.h>

// unsigned char cipher[] = {105, 80, 5, 211, 28, 89, 191, 207, 116, 142, 193, 34, 172, 155, 154, 215, 32, 97, 32, 109, 97, 110, 32, 112, 117, 116, 32, 97, 32, 103, 117, 110, 32, 97, 103, 97, 105, 110, 115, 116, 32, 104, 105, 115, 32, 104, 101, 97, 100, 32, 112, 117, 108, 108, 101, 100, 32, 109, 121, 32, 116, 114, 105, 103, 103, 101, 114, 0, 72, 12, 1, 0, };
unsigned char cipher[] = {128, 236, 120, 132, 92, 79, 112, 205, 223, 124, 253, 232, 127, 5, 237, 202, 90, 255, 214, 175, 207, 40, 222, 70, 235, 37, 181, 152, 85, 71, 221, 149, 127, 83, 92, 164, 30, 112, 233, 182, 161, 226, 125, 101, 254, 67, 99, 4, 201, 19, 128, 13, 184, 230, 84, 29, 167, 44, 194, 132, 119, 217, 57, 129, 119, 155, 201, 171, 132, 179, 97, 140, };

char search[] = "man";
//----------------------------------------------------------
void decrypt (long key, char *ciph, int len)
{
  // prepare key for the parity calculation. Least significant bit in all bytes should be empty
  long k = 0;
  for (int i = 0; i < 8; i++)
    {
      key <<= 1;
      k += (key & (0xFE << i * 8));
    }

  // Decrypt ciphertext
  des_setparity ((char *) &k);
  ecb_crypt ((char *) &k, (char *) ciph, 16, DES_DECRYPT);
}
//----------------------------------------------------------
// Returns true if the plaintext produced containes the "search" string
bool tryKey (long key, char *ciph, int len)
{
  char temp[len + 1];
  memcpy (temp, ciph, len);
  temp[len] = 0;
  decrypt (key, temp, len);
  return strstr ((char *) temp, search) != NULL;
}
//----------------------------------------------------------
int main (int argc, char **argv)
{
  int N, id;
  long long upper=(1LL << 56);  
  long found = 0;
  MPI_Status st;
  MPI_Request req;
  int flag;
  int ciphLen = strlen((char *)cipher);
  
  MPI_Init (&argc, &argv);
  double start = MPI_Wtime();
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &N);
  MPI_Irecv ((void *)&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
  int iterCount=0;
  for (long long i = id; i < upper; i += N)
    {
      if (tryKey (i, (char *) cipher, ciphLen))
        {
          found = i;
          for (int node = 0; node < N; node++)
            MPI_Send ((void *)&found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
          break;
        }
      if(++iterCount % 1000 == 0) // check the status of the pending receive
      {
        MPI_Test(&req, &flag, &st);
        if(flag) break;
      }
    }

  if (id == 0)
    {
      MPI_Wait (&req, &st); // in case process 0 finishes before the key is found     
      decrypt (found, (char *) cipher, ciphLen);
      printf ("Found using %i nodes in %lf sec!\n", N, MPI_Wtime()- start);
      usleep(1000000);
      printf ("%li %s\n", found, cipher);
    }
  MPI_Finalize ();
}