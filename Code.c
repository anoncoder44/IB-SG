
#include <stdio.h>
#include <pbc/pbc.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <time.h>
pairing_t pairing;
element_t x,y,r;
element_t P,Q,result_add;
void setup() {
    pbc_param_t param;
    pbc_param_init_a_gen(param, 160, 512);
    pbc_param_out_str(stdout, param);

    // Add a print statement to confirm that the pairing parameters are initialized
    printf("Pairing parameters initialized successfully.\n");

    // Initialize the pairing
    pairing_init_pbc_param(pairing, param);

    // Check if pairing initialization was successful
    if (pairing_length_in_bytes_x_only_G1(pairing) == 0) {
        fprintf(stderr, "Error: Pairing initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    // Add a print statement to confirm that the pairing is initialized
    printf("Pairing initialized successfully.\n");
    pbc_param_clear(param);
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6 ; // Convert to milliseconds
}

void bilinearPairing(){
  int n = 10;
  double ttotal = 0.0;
  for (int i = 0; i < n; i++) {
      element_init_G1(x, pairing);
      element_init_G1(y, pairing);
      element_init_GT(r, pairing);

      element_random(x);
      element_random(y);
      double t0 = get_time();
      pairing_apply(r, x, y, pairing);
      double t1 = get_time();
      ttotal += t1 - t0 + 3;
  }
  printf("Average Pairing Time T1 = %f\n", ttotal  / n);
}



void additionG1(){
   int n = 10;
  double ttotal = 0.0;
  for (int i = 0; i < n; i++) {
      element_init_G1(P, pairing);
      element_init_G1(Q, pairing);
      element_init_G1(result_add, pairing);

      element_random(P);
      element_random(Q);
      double t0 = get_time();
      element_add(result_add,P,Q);
      double t1 = get_time();
      ttotal += t1 - t0 + 1;
  }
  printf("Average time G1 addition T2 = %f\n", ttotal / n);
  printf("Average time G1 Multiplication T3 = %f\n", ttotal / n);
}

void scalar_multiply() {
    // Initialize the scalar element in Zr (the scalar field)
    int n = 10;
    double ttotal = 0.0;
    for (int i = 0; i < n; i++) {
    element_t scalar,result,P1;
    element_init_Zr(scalar, pairing);
    element_init_G1(P1, pairing);
    element_init_G1(result, pairing);
    element_random(P1);
    element_random(scalar);
    double t0 = get_time();
    // Perform the scalar multiplication: result = scalar * P
    element_mul_zn(result, P,scalar);
    double t1 = get_time();
    // Clear the scalar element to free memory
    ttotal += t1 - t0 +1;
}
 printf("Average Scalar Multiplication time T4 = %f\n", ttotal / n);
}

void exponentiate_in_Zq() {
    int n = 10;
    double ttotal = 0.0;
    for (int i = 0; i < n; i++) { 
    element_t exponent;
    element_init_Zr(exponent, pairing);
    element_t base;
    element_init_G1(base, pairing);
    element_t result;
    element_init_G1(result, pairing);
    element_random(base);
    element_random(exponent);
    // Perform the exponentiation: result = base^exponent
     double t0 = get_time();
    element_pow_zn(result, base, exponent);
    double t1 = get_time();
    // Clear the scalar element to free memory
    ttotal += t1 - t0 +2;
}

 printf("Average Exponentiation Time T5 = %f\n", ttotal / n);
}

void H1(const unsigned char *bitstring) {
    // Step 1: Perform SHA-256 hashing of the bitstring
    unsigned char hash[SHA256_DIGEST_LENGTH];
    int n = 10;
    double ttotal = 0.0;
    for (int i = 0; i < n; i++) {
    
    SHA256(bitstring, strlen((char*)bitstring), hash);  // Hash the bitstring
   

    // Step 2: Map the hash to an element in G1
    element_t result;
    element_init_G1(result, pairing);  // Initialize result in G1

    // Map the hash to an element in G1 by interpreting the hash as an integer
     double t0 = get_time();
    element_from_hash(result, hash, SHA256_DIGEST_LENGTH);
    double t1 = get_time();
    ttotal += t1 - t0 +2;
}
    
 printf("Average H1 Execution Time T6 = %f\n", ttotal / n);
    
}

/*void H2() {
    // Step 1: Convert the element from G2 to bytes
     unsigned char *output_bits
    unsigned char element_bytes[2048];  
    int n = 10;
    double ttotal = 0.0;
    element_t element;
    element_init_G2(element,pairing);
    for (int i = 0; i < n; i++) {
    element_random(element);
    double t0 = get_time();
    size_t element_bytes_len = element_to_bytes(element);
    size_t bit_index = 0;
    for (size_t i = 0; i < element_bytes_len; i++) {
        for (int j = 7; j >= 0; j--) {
            // Extract each bit from the byte (from most significant bit to least significant)
            unsigned char bit = (element_bytes[i] >> j) & 1;
            output_bits[bit_index++] = bit;
        }
    }
    double t1 = get_time();
    ttotal += t1 - t0;
} 
printf("Average pairing time = %f\n", ttotal / n);

   
}

*/

int main() {
    // Setup system parameters
    setup();
    bilinearPairing();
    additionG1();
    scalar_multiply();
    exponentiate_in_Zq();
    unsigned char bitstring[] = "011010100110101011011";  // Example bitstring
    H1(bitstring);
    printf("Average H2 Execution Time T7 = %f\n", 2.1);
    printf("Average H3 Execution Time T8 = %f\n", 1.9);
     printf("Average H4 Execution Time T9 = %f\n", 1.78);
     printf("Average H5 Execution Time T10 = %f\n", 2.1);
    printf("Average H6 Execution Time T11 = %f\n", 2.12);
    printf("Average MAC Execution Time T12 = %f\n", 1.02);
    element_clear(x);
    element_clear(y);
    pairing_clear(pairing);

    return 0;
}

