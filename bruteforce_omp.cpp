/*
 ============================================================================
 Author(s)    : G. Barlas, Gus Mendez, Roberto Figueroa, Michele Benvenuto
 
 Compile      : mpic++ -fopenmp -o bruteforce2 bruteforce2.cpp 
 ============================================================================
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpc/des_crypt.h>
#include <mpi.h>
#include <omp.h>

int BLOCK = 1000000;

// unsigned char cipher[] = { 142, 104, 132, 216, 225, 216, 111, 227, 143, 206, 198, 251, 229, 140, 89, 74, 32, 115, 97, 118, 101, 32, 111, 117, 114, 115, 101, 108, 118, 101, 115, 0 }; 
unsigned char cipher[] = {128, 236, 120, 132, 92, 79, 112, 205, 223, 124, 253, 232, 127, 5, 237, 202, 90, 255, 214, 175, 207, 40, 222, 70, 235, 37, 181, 152, 85, 71, 221, 149, 127, 83, 92, 164, 30, 112, 233, 182, 161, 226, 125, 101, 254, 67, 99, 4, 201, 19, 128, 13, 184, 230, 84, 29, 167, 44, 194, 132, 119, 217, 57, 129, 119, 155, 201, 171, 132, 179, 97, 140, };

char search[] = "man";
void decrypt (long key, char *ciph, int len)
{
  long k = 0;
  for (int i = 0; i < 8; i++)
    {
      key <<= 1;
      k += (key & (0xFE << i * 8));
    }

  des_setparity ((char *) &k);
  ecb_crypt ((char *) &k, (char *) ciph, len, DES_DECRYPT);
}

bool tryKey (long key, char *ciph, int len)
{
  char temp[len + 1];
  memcpy (temp, ciph, len);
  temp[len] = 0;
  decrypt (key, temp, len);
  return strstr ((char *) temp, search) != NULL;
}

int main (int argc, char **argv)
{
  int N, id;
  long long upper = (1LL << 56);
  long found = -1;
  MPI_Status st;
  MPI_Request req;
  int flag = 0;
  int ciphLen = strlen ((char *) cipher);

  // BLOCK = atoi(argv[1]);

  MPI_Init (&argc, &argv);
  double start = MPI_Wtime ();
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &N);
  MPI_Irecv ((void *) &found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
  int iterCount = 0;

  long idx = 0;
  while (idx < upper && found < 0)
    {
      #pragma omp parallel for default(none) shared(cipher, ciphLen, found, idx, id, N, BLOCK)
      for (long long i = idx + id; i < idx + N * BLOCK; i += N)
        {
          if (tryKey (i, (char *) cipher, ciphLen))
            {
              #pragma omp critical
              found = i;
            }
        }

      if (found >= 0)
        {
          for (int node = 0; node < N; node++)
            MPI_Send ((void *) &found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
        }

      idx += N * BLOCK;
    }

  if (id == 0)
    {
      MPI_Wait (&req, &st);         
      decrypt (found, (char *) cipher, ciphLen);
      printf ("Found using %i nodes in %lf sec!\n", N, MPI_Wtime()- start);
      usleep(1000000);
      printf ("%li %s\n", found, cipher);
    }
  MPI_Finalize ();
}