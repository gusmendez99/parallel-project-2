/*
	Code Reference: https://cryptopp.com/wiki/TripleDES
    For cryptographic purposes a Crypto++ library was used (https://github.com/weidai11/cryptopp). 
*/

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

#include <cryptopp/des.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/secblock.h>

using CryptoPP::AutoSeededRandomPool;
using CryptoPP::Exception;
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::DES;
using CryptoPP::CBC_Mode;
using CryptoPP::SecByteBlock;
using CryptoPP::byte;

#define UPPER_LIMIT 255
#define LOWER_LIMIT 0
#define STOP_SEARCH_COND 

//! Hard-coded message to encrypt. It is used to validate the decryption output in brute-force method.
const string PLAIN_TEXT = "A brute-force attack is a cryptanalytic attack that can be used to attempt to decrypt any encrypted data.";
const string PLAIN_TEXT_SEARCH = "attack";


int main(int argc, char* argv[])
{
	AutoSeededRandomPool prng;

	SecByteBlock key(DES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock(key, key.size());
	// Ir you dont want a PRNG, use this: 
	// byte key[DES::KEYLENGTH] = {CHAR_MIN, CHAR_MIN, CHAR_MIN, CHAR_MIN, CHAR_MIN, CHAR_MIN, CHAR_MIN, CHAR_MIN};

	byte iv[DES::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));

	// Plain & Stop Search Condition
	string cipher, encoded, candidate;
	

	/*********************************\
	 * 
	 * 			Encryption
	 * 
	\*********************************/
	
	// Pretty print key
	encoded.clear();
	StringSource(key, key.size(), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "key: " << encoded << endl;

	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "iv: " << encoded << endl;

	try
	{
		cout << "plain text: " << PLAIN_TEXT << endl;

		CBC_Mode< DES >::Encryption e;
		e.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter adds padding
		//  as required. ECB and CBC Mode must be padded
		//  to the block size of the cipher.
		StringSource(PLAIN_TEXT, true, 
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter      
		); // StringSource
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	 * 
	 * 	  Decryption (Brute Force)
	 * 
	\*********************************/

	// Pretty print
	encoded.clear();
	StringSource(cipher, true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "cipher text: " << encoded << endl;

	/*********************************\
	\*********************************/
	
	CryptoPP::byte key_bf[DES::KEYLENGTH];
	for(int i = 0; i < 8; i++) {
		key_bf[i] = (CryptoPP::byte) 0;
	};

	try
	{
		for(int i = 0; i < 255; i++){
			key_bf[0] = (byte) i;
			for(int j = 0; j < 255; j++){
				key_bf[1] = (byte) j;
				for(int k = 0; k < 255; k++){
					key_bf[2] = (byte) k;
					for(int l = 0; l < 255; l++){
						key_bf[3] = (byte) l;
						for(int m = 0; m < 255; m++){
							key_bf[4] = (byte) m;
							for(int n = 0; n < 255; n++){
								key_bf[5] = (byte) n;
								for(int o = 0; o < 255; o++){
									key_bf[6] = (byte) o;
									for(int p = 0; p < 255; p++){
										key_bf[7] = (byte) p;

										CBC_Mode<DES>::Decryption d;
										d.SetKeyWithIV(key_bf, DES::KEYLENGTH, iv);

										// The StreamTransformationFilter removes
										//  padding as required.
										StringSource s(cipher, true, 
											new StreamTransformationFilter(d,
												new StringSink(candidate),
												CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING 
											) // StreamTransformationFilter
										); // StringSource

										// Check Stop Search Condition
										if(candidate.find(PLAIN_TEXT_SEARCH) != string::npos)
    									{
											cout << "DES was successfully broken. Decipher text: " << candidate << endl;
											return 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}

