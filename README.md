# DES

Project 2 - Parallel Programming

CC3069 class

## Tools & Requirements 🔧

- C++
- OpenMPI (and OpenMP)
- RPC (DES utility inside library was deprecated... so, we had to use old OS ~ 2016)
- 2 RPI 4 model B, running old Raspbian version (Jessie)

Code based on the examples of "Multicore and GPU Programming: An Integrated Approach", by Gerassimos Barlas

`Barlas, G. (2014). Multicore and GPU Programming: An integrated approach. Elsevier.`

## Test and run 💻

### RPC initial tests
#### Generate a new cipher text

Compile

```console
g++ -fpermissive -std=c++11 -o encrypt encrypt.cpp
```

Run

```console
./encrypt "PLAIN_TEXT_HERE" [2^56 key]
```

#### Test DES encrypt/decrypt from TXT file

Compile

```console
g++ -o example example.cpp
```

Run

```console
./example
```




### Parallel

#### Sequential (using N=1)

Compile

```console
mpic++ -o bruteforce bruteforce.cpp
```

Run

```console
./mpirun -np 1 bruteforce
```


#### Parallel (v2)

Compile

```console
mpic++ -fpermissive -o bruteforcev2 bruteforcev2.cpp
```

Run

```console
./mpirun -np N bruteforcev2
```

#### Parallel OMP (v3)

Compile

```console
mpic++ -fopenmp -o bruteforce_omp bruteforce_omp.cpp
```

Run

```console
./mpirun -np N bruteforce_omp
```

### Extra: Clusters

This is our second time doing clusters with multiple RPI...

![clusters](https://github.com/gusmendez99/parallel-project-2/blob/main/images/clusters.png?raw=true)



|                     	| **RPI 4 B (2 cores)** 	| **RPI 4 B with RPI 1** 	|
|---------------------	|:----------------------:	|:----------------------:	|
|       **AVG Time** 	|        0.6068708s       	|        1.1612702s       	|
|     **Speedup**     	|          5331          	|          2786          	|

