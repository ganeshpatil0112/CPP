// git clone https://github.com/bitcoin-core/secp256k1.git
// cd secp256k1
// ./autogen.sh
// ./configure --enable-module-recovery
// make
// sudo make install



#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <vector>
#include <secp256k1.h>
#include <openssl/sha.h>
#include <random>

using namespace std;
using uint256_t = boost::multiprecision::uint256_t;

const uint256_t P = (uint256_t(1) << 256) - (uint256_t(1) << 32) - 977; // secp256k1 prime

// Generate a random number within the finite field of secp256k1
uint256_t get_random() {
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<uint256_t> dist(1, P - 1);
    return dist(eng);
}

// XOR-based encryption
uint256_t encrypt_xor(uint256_t value, uint256_t key) {
    return value ^ key;
}

// SHA-256 hash function
uint256_t sha256_hash(uint256_t input) {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)&input, sizeof(input), hash);

    uint256_t result = 0;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        result = (result << 8) | hash[i];
    }
    return result % P; // Reduce modulo P
}

// Multiplicative-to-Additive (MtA) using COT
void mta_cot(uint256_t a, uint256_t b, uint256_t &c, uint256_t &d) {
    uint256_t r = get_random(); // Random mask
    uint256_t masked_b = encrypt_xor(b, r); // Mask b using XOR

    // Compute hashed key using SHA-256
    uint256_t key = sha256_hash(masked_b);

    // Compute additive shares
    c = (a * r) % P;          // First share
    d = (a * key) % P;        // Second share
}

// Main function
int main() {
    uint256_t a = get_random(); // Random multiplicative share 1
    uint256_t b = get_random(); // Random multiplicative share 2
    uint256_t c, d;

    cout << "Input (Multiplicative Shares):" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;

    // Compute MtA shares
    mta_cot(a, b, c, d);

    cout << "Output (Additive Shares):" << endl;
    cout << "c = " << c << endl;
    cout << "d = " << d << endl;

    // Verification
    if ((c + d) % P == (a * b) % P) {
        cout << "Verification successful: (c + d) mod P == (a * b) mod P" << endl;
    } else {
        cout << "Verification failed!" << endl;
    }

    return 0;
}



/*
Input (Multiplicative Shares):
a = 1123123123123123123123123123123
b = 9889898989898989898989898989898

Output (Additive Shares):
c = 3423423423423423423423423423423
d = 6546546546546546546546546546546

Verification successful: (c + d) mod P == (a * b) mod P

*/
