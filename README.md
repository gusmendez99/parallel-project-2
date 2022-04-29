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
g++ -std=c++11 -DNDEBUG -g3 -O2 -Wall -Wextra -o sequential sequential.cpp -l:libcryptopp.a
```

Run it

```console
./sequential
```

### Parallel

[WIP]