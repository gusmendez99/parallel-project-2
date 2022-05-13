/*
 ============================================================================
 Author(s)    : Miguel
 
 Compile      : mpic++ -o bruteforce bruteforce.cpp 
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
  ecb_crypt((char *)&k, (char *) ciph, 16, DES_DECRYPT);
}

void encrypt(long key, char *ciph, int len){
  //set parity of key and do decrypt
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);  //el poder del casteo y &
  ecb_crypt((char *)&k, (char *) ciph, 16, DES_ENCRYPT);
}

char search[] = "pulled";
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

unsigned char cipher[] = {105, 80, 5, 211, 28, 89, 191, 207, 116, 142, 193, 34, 172, 155, 154, 215, 32, 97, 32, 109, 97, 110, 32, 112, 117, 116, 32, 97, 32, 103, 117, 110, 32, 97, 103, 97, 105, 110, 115, 116, 32, 104, 105, 115, 32, 104, 101, 97, 100, 32, 112, 117, 108, 108, 101, 100, 32, 109, 121, 32, 116, 114, 105, 103, 103, 101, 114, 0, 72, 12, 1, 0, };
int main(int argc, char *argv[]){ //char **argv
  int N, id;
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

  for(long i = mylower; i<myupper; ++i){
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
    printf("%li %s\n", found, cipher);
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