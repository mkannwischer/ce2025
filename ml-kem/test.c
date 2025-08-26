#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "api.h"
#include "hal.h"
#include "../common/randombytes.h"
#include "poly.h"

#include "testvectors.inc"


static int test_keygen_vector(void)
{
    uint8_t pk[pqcrystals_kyber768_ref_PUBLICKEYBYTES];
    uint8_t sk[pqcrystals_kyber768_ref_SECRETKEYBYTES];
    int i;
    
    hal_send_str("\n=== Test 1: Keypair Generation ===\n");
    
    // Generate keypair with test vector coins
    if(pqcrystals_kyber768_ref_keypair_derand(pk, sk, tv_keypair_coins) != 0) {
        hal_send_str("Keypair generation failed!\n");
        return -1;
    }
    
    // Compare public key
    for(i = 0; i < pqcrystals_kyber768_ref_PUBLICKEYBYTES; i++) {
        if(pk[i] != tv_expected_pk[i]) {
            hal_send_str("Public key mismatch!\n");
            return -1;
        }
    }
    
    // Compare secret key
    for(i = 0; i < pqcrystals_kyber768_ref_SECRETKEYBYTES; i++) {
        if(sk[i] != tv_expected_sk[i]) {
            hal_send_str("Secret key mismatch!\n");
            return -1;
        }
    }
    
    hal_send_str("✓ Keypair generation test vector PASSED\n");
    return 0;
}

static int test_encaps_vector(void)
{
    uint8_t ct[pqcrystals_kyber768_ref_CIPHERTEXTBYTES];
    uint8_t ss[pqcrystals_kyber768_ref_BYTES];
    int i;
    
    hal_send_str("\n=== Test 2: Encapsulation ===\n");
    
    // Perform encapsulation with test vector public key and coins
    if(pqcrystals_kyber768_ref_enc_derand(ct, ss, tv_encaps_pk, tv_encaps_coins) != 0) {
        hal_send_str("Encapsulation failed!\n");
        return -1;
    }
    
    // Compare ciphertext
    for(i = 0; i < pqcrystals_kyber768_ref_CIPHERTEXTBYTES; i++) {
        if(ct[i] != tv_expected_ct[i]) {
            hal_send_str("Ciphertext mismatch!\n");
            return -1;
        }
    }
    
    // Compare shared secret
    for(i = 0; i < pqcrystals_kyber768_ref_BYTES; i++) {
        if(ss[i] != tv_expected_ss_encaps[i]) {
            hal_send_str("Shared secret mismatch!\n");
            return -1;
        }
    }
    
    hal_send_str("✓ Encapsulation test vector PASSED\n");
    return 0;
}

static int test_decaps_vector(void)
{
    uint8_t ss[pqcrystals_kyber768_ref_BYTES];
    int i;
    
    hal_send_str("\n=== Test 3: Decapsulation ===\n");
    
    // Perform decapsulation with test vector secret key and ciphertext
    if(pqcrystals_kyber768_ref_dec(ss, tv_decaps_ct, tv_decaps_sk) != 0) {
        hal_send_str("Decapsulation failed!\n");
        return -1;
    }
    
    // Compare shared secret
    for(i = 0; i < pqcrystals_kyber768_ref_BYTES; i++) {
        if(ss[i] != tv_expected_ss_decaps[i]) {
            hal_send_str("Shared secret mismatch!\n");
            return -1;
        }
    }
    
    hal_send_str("✓ Decapsulation test vector PASSED\n");
    return 0;
}

static int run_test(void)
{
    uint8_t pk[pqcrystals_kyber768_ref_PUBLICKEYBYTES];
    uint8_t sk[pqcrystals_kyber768_ref_SECRETKEYBYTES];
    uint8_t ct[pqcrystals_kyber768_ref_CIPHERTEXTBYTES];
    uint8_t ss1[pqcrystals_kyber768_ref_BYTES];
    uint8_t ss2[pqcrystals_kyber768_ref_BYTES];
    uint8_t coins_keypair[pqcrystals_kyber768_ref_KEYPAIRCOINBYTES];
    uint8_t coins_enc[pqcrystals_kyber768_ref_ENCCOINBYTES];
    int i;


    // Generate deterministic randomness for testing
    for(i = 0; i < pqcrystals_kyber768_ref_KEYPAIRCOINBYTES; i++) {
        coins_keypair[i] = i;
    }
    for(i = 0; i < pqcrystals_kyber768_ref_ENCCOINBYTES; i++) {
        coins_enc[i] = i + 64;
    }

    // Generate keypair
    if(pqcrystals_kyber768_ref_keypair_derand(pk, sk, coins_keypair) != 0) {
        hal_send_str("Keypair generation failed!\n");
        return -1;
    }

    // Encapsulation
    if(pqcrystals_kyber768_ref_enc_derand(ct, ss1, pk, coins_enc) != 0) {
        hal_send_str("Encapsulation failed!\n");
        return -1;
    }

    // Decapsulation
    if(pqcrystals_kyber768_ref_dec(ss2, ct, sk) != 0) {
        hal_send_str("Decapsulation failed!\n");
        return -1;
    }

    // Compare shared secrets
    for(i = 0; i < pqcrystals_kyber768_ref_BYTES; i++) {
        if(ss1[i] != ss2[i]) {
            hal_send_str("Shared secrets don't match!\n");
            return -1;
        }
    }

    hal_send_str("✓ Functional KEM test PASSED\n");
    return 0;
}

#ifndef MPS2_AN386
static void run_speed(void)
{
    uint8_t pk[pqcrystals_kyber768_ref_PUBLICKEYBYTES];
    uint8_t sk[pqcrystals_kyber768_ref_SECRETKEYBYTES];
    uint8_t ct[pqcrystals_kyber768_ref_CIPHERTEXTBYTES];
    uint8_t ss[pqcrystals_kyber768_ref_BYTES];
    poly a;
    uint64_t cycles;
    char cycles_str[32];

    hal_send_str("\n=== Benchmarks ===\n");

    // poly_ntt benchmark
    cycles = hal_get_time();
    poly_ntt(&a);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for poly_ntt: ");
    sprintf(cycles_str, "%llu\n", (unsigned long long)cycles);
    hal_send_str(cycles_str);

    // Keypair generation benchmark
    cycles = hal_get_time();
    pqcrystals_kyber768_ref_keypair(pk, sk);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for keypair generation: ");
    sprintf(cycles_str, "%llu\n", (unsigned long long)cycles);
    hal_send_str(cycles_str);

    // Encapsulation benchmark
    cycles = hal_get_time();
    pqcrystals_kyber768_ref_enc(ct, ss, pk);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for encapsulation: ");
    sprintf(cycles_str, "%llu\n", (unsigned long long)cycles);
    hal_send_str(cycles_str);

    // Decapsulation benchmark
    cycles = hal_get_time();
    pqcrystals_kyber768_ref_dec(ss, ct, sk);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for decapsulation: ");
    sprintf(cycles_str, "%llu\n", (unsigned long long)cycles);
    hal_send_str(cycles_str);
}
#endif

int main(void)
{
    hal_setup(CLOCK_BENCHMARK);

    // First test: verify keypair generation test vectors
    int test_result = test_keygen_vector();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }
    
    // Second test: verify encapsulation test vectors
    test_result = test_encaps_vector();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }
    
    // Third test: verify decapsulation test vectors
    test_result = test_decaps_vector();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }
    
    // Fourth test: functional test
    hal_send_str("\n=== Test 4: Functional KEM Test ===\n");
    test_result = run_test();

#ifndef MPS2_AN386
    run_speed();
#endif

    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }

    hal_send_str("\n*** ALL GOOD ***\n");
    return 0;
}