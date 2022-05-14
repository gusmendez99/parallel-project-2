/*
 ============================================================================
 Author(s)    : Miguel
 
 Compile      : mpic++ -fpermissive -o bruteforce bruteforce.cpp 
 ============================================================================
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <rpc/des_crypt.h>

/*
* Decrypt
*    Parámetros:
* key:  Llave para descifrar
* ciph: Puntero al texto cifrado
* len: Largo del texto a descifrar
*     Funcionamiento: Esta función permite descifrar un texto cifrado con una llave. 
*
*/
void decrypt(long key, char *ciph, int len){
  //set parity of key and do decrypt
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);  //el poder del casteo y &
  ecb_crypt((char *)&k, (char *) ciph, len, DES_DECRYPT);
}

void encrypt(long key, char *ciph, int len){
  //set parity of key and do decrypt
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);  //el poder del casteo y &
  ecb_crypt((char *)&k, (char *) ciph, len, DES_ENCRYPT);
}

char search[] = "man";
/*
* tryKey
*     Parámetros: 
* key: Llave que se probara para descifrar el texto 
* ciph: Puntero al texto cifrado
* len: Largo del texto cifrado
* Funcionamiento: Está función recibe el texto cifrado y la llave para realizar el descifrado del texto luego de lo cual busca una palabra clave predefinida con el objetivo de determinar si se logró o no se logró descifrar el texto, Regresa el índice en el cual empieza la palabra clave o null si no se encuentra  
*/

int tryKey(long key, char *ciph, int len){
  char temp[len+1];
  
  memcpy(temp, ciph, len);
  temp[len]=0;
  decrypt(key, temp, len);
  return strstr((char *)temp, search) != NULL;
}

// unsigned char cipher[] = {105, 80, 5, 211, 28, 89, 191, 207, 116, 142, 193, 34, 172, 155, 154, 215, 32, 97, 32, 109, 97, 110, 32, 112, 117, 116, 32, 97, 32, 103, 117, 110, 32, 97, 103, 97, 105, 110, 115, 116, 32, 104, 105, 115, 32, 104, 101, 97, 100, 32, 112, 117, 108, 108, 101, 100, 32, 109, 121, 32, 116, 114, 105, 103, 103, 101, 114, 0, 72, 12, 1, 0, };
// unsigned char cipher[] = {97, 68, 115, 120, 1, 35, 19, 175, 101, 102, 248, 68, 170, 181, 115, 177, 155, 79, 143, 21, 88, 208, 125, 114, 228, 123, 3, 77, 111, 253, 111, 74, 111, 112, 144, 160, 159, 17, 235, 92, 40, 76, 110, 33, 83, 79, 193, 140, 73, 246, 11, 75, 152, 42, 127, 158, 9, 229, 99, 23, 24, 47, 91, 49, 207, 55, 115, 208, 170, 145, 116, 102, };
unsigned char cipher[] = {128, 236, 120, 132, 92, 79, 112, 205, 223, 124, 253, 232, 127, 5, 237, 202, 90, 255, 214, 175, 207, 40, 222, 70, 235, 37, 181, 152, 85, 71, 221, 149, 127, 83, 92, 164, 30, 112, 233, 182, 161, 226, 125, 101, 254, 67, 99, 4, 201, 19, 128, 13, 184, 230, 84, 29, 167, 44, 194, 132, 119, 217, 57, 129, 119, 155, 201, 171, 132, 179, 97, 140, };

int main(int argc, char *argv[]){ //char **argv
  int N, id;
  double start = MPI_Wtime();
  long long upper = (1LL <<56); //upper bound DES keys 2^56
  long long mylower, myupper;
  MPI_Status st;
  MPI_Request req;
  int flag;
  int ciphlen = strlen(cipher);
  MPI_Comm comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &N);
  MPI_Comm_rank(comm, &id);

  long range_per_node = upper / N;
  mylower = range_per_node * id;
  myupper = range_per_node * (id+1) -1;
  if(id == N-1){
    //compensar residuo
    myupper = upper;
  }

  long found = 0;
  int ready = 0;

  MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);

  for(long long i = mylower; i<myupper; ++i){
    MPI_Test(&req, &ready, MPI_STATUS_IGNORE);
    if(ready)
      break;  //ya encontraron, salir

    if(tryKey(i, (char *)cipher, ciphlen)){
      found = i;
      for(int node=0; node<N; node++){
        MPI_Send(&found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
      }
      break;
    }
  }

  if(id==0){
    MPI_Wait(&req, &st);
    decrypt(found, (char *)cipher, ciphlen);
    printf ("Found in %lf sec : %s\n", MPI_Wtime()- start, cipher);
  }

  MPI_Finalize();
}


// Descripción de funciones utlizadas dentro del código:

/*
* memcpy
*     Parámetros:
* dest: El puntero destino del arreglo donde el contenido será copiado
* src: El puntero donde la data de origen se encuentra
* n: La cantidad de bytes a ser copiada
*     Funcionamiento:
*     Copia n caracteres de una localidad de memoria src a una localidad diferente de memoria dest.
*/


/*

* strstr
*     Parámetros:
* str: Puntero al string en el cual se desea buscar un substring 
* target: Puntero al string que se quiere encontrar dentro del string 
* Funcionamiento: Está función retorna un puntero a la posición en la cual se encuentra la primera letra de str dentro de target, si no se encuentra retorna un puntero a NULL.  
*/