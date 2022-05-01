# DES

Project 2 - Parallel Programming

CC3069 class

## Tools & Requirements 🔧

- C++
- OpenMPI
- Crypto++ (See [installation](https://www.cryptopp.com/wiki/Linux#apt-get))


## Test and run 💻

### Sequential

Compile

```console
g++ -g3 -O2 -Wall -Wextra -o sequential sequential.cpp -l:libcryptopp.a
```

Run it

```console
./sequential
```

### Parallel (C++11 threads)

Compile

```console
g++ -g3 -O2 -Wall -Wextra -o parallel parallel.cpp -l:libcryptopp.a
```

Run it

```console
./parallel
```

### Parallel MPI (with OpenMPI)

[WIP]