#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "hal.h"
#include "randombytes.h"
#include "sign.h"
#include "params.h"

#include "testvectors.inc"

static int test_keygen_vector(void)
{
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    int i;

    hal_send_str("\n=== Test 1: Keypair Generation ===\n");

    // Generate keypair with test vector seed
    if(crypto_sign_keypair_internal(pk, sk, tv_keypair_seed) != 0) {
        hal_send_str("Keypair generation failed!\n");
        return -1;
    }

    // Compare public key with expected NIST vector
    for(i = 0; i < CRYPTO_PUBLICKEYBYTES; i++) {
        if(pk[i] != tv_expected_pk[i]) {
            hal_send_str("Public key mismatch!\n");
            return -1;
        }
    }

    // Compare secret key with expected NIST vector
    for(i = 0; i < CRYPTO_SECRETKEYBYTES; i++) {
        if(sk[i] != tv_expected_sk[i]) {
            hal_send_str("Secret key mismatch!\n");
            return -1;
        }
    }

    hal_send_str("✓ Keypair generation test vector PASSED\n");
    return 0;
}

static int test_signature_vector(void)
{
    uint8_t sig[CRYPTO_BYTES];
    uint8_t pre[257]; // Maximum prefix size (0 + ctxlen + ctx)
    size_t siglen;
    size_t ctxlen = sizeof(tv_sign_ctx);
    size_t i;

    hal_send_str("\n=== Test 2: Signature Generation ===\n");

    // Construct prefix as done in crypto_sign_signature: pre = (0, ctxlen, ctx)
    pre[0] = 0;                    // Always 0 for regular signing
    pre[1] = ctxlen;               // Length of context
    for(i = 0; i < ctxlen; i++) {  // Copy context data
        pre[2 + i] = tv_sign_ctx[i];
    }

    // Sign message using internal function with test vector inputs
    if(crypto_sign_signature_internal(sig, &siglen,
                                     tv_sign_message, sizeof(tv_sign_message),
                                     pre, 2 + ctxlen,
                                     tv_sign_rnd, tv_sign_sk) != 0) {
        hal_send_str("Signature generation failed!\n");
        return -1;
    }

    // Compare signature
    for(i = 0; i < CRYPTO_BYTES; i++) {
        if(sig[i] != tv_expected_sig[i]) {
            hal_send_str("Signature mismatch!\n");
            return -1;
        }
    }

    hal_send_str("✓ Signature generation test vector PASSED\n");
    return 0;
}

static int test_valid_signature_verification(void)
{
    uint8_t pre[257]; // Maximum prefix size (0 + ctxlen + ctx)
    size_t ctxlen = sizeof(tv_verify_pos_ctx);
    size_t i;
    int result;

    hal_send_str("\n=== Test 3: Valid Signature Verification ===\n");

    // Construct prefix as done in crypto_sign_verify: pre = (0, ctxlen, ctx)
    pre[0] = 0;                    // Always 0 for regular signing
    pre[1] = ctxlen;               // Length of context
    for(i = 0; i < ctxlen; i++) {  // Copy context data
        pre[2 + i] = tv_verify_pos_ctx[i];
    }

    // Verify signature using internal function with test vector inputs
    result = crypto_sign_verify_internal(tv_verify_pos_sig, CRYPTO_BYTES,
                                        tv_verify_pos_message, sizeof(tv_verify_pos_message),
                                        pre, 2 + ctxlen,
                                        tv_verify_pos_pk);

    if(result != 0) {
        hal_send_str("Valid signature verification failed (unexpected)!\n");
        return -1;
    }

    hal_send_str("✓ Valid signature verification test vector PASSED\n");
    return 0;
}

static int test_invalid_signature_verification(void)
{
    uint8_t pre[257]; // Maximum prefix size (0 + ctxlen + ctx)
    size_t ctxlen = sizeof(tv_verify_neg_ctx);
    size_t i;
    int result;

    hal_send_str("\n=== Test 4: Invalid Signature Verification ===\n");

    // Construct prefix as done in crypto_sign_verify: pre = (0, ctxlen, ctx)
    pre[0] = 0;                    // Always 0 for regular signing
    pre[1] = ctxlen;               // Length of context
    for(i = 0; i < ctxlen; i++) {  // Copy context data
        pre[2 + i] = tv_verify_neg_ctx[i];
    }

    // Verify signature using internal function with test vector inputs
    result = crypto_sign_verify_internal(tv_verify_neg_sig, CRYPTO_BYTES,
                                        tv_verify_neg_message, sizeof(tv_verify_neg_message),
                                        pre, 2 + ctxlen,
                                        tv_verify_neg_pk);

    if(result == 0) {
        hal_send_str("Invalid signature verification passed (unexpected)!\n");
        return -1;
    }

    hal_send_str("✓ Invalid signature verification test vector PASSED (verification failed as expected)\n");
    return 0;
}


static int test_functional(void)
{
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];
    uint8_t message[53];
    uint8_t rnd[RNDBYTES];
    uint8_t ctx[1] = {0}; // Empty context
    size_t siglen;
    int i;

    hal_send_str("\n=== Test 5: Functional Test ===\n");

    // Create deterministic 53-byte zero message
    memset(message, 0, 53);

    // Create deterministic randomness for testing
    for(i = 0; i < RNDBYTES; i++) {
        rnd[i] = i;
    }

    // Generate keypair
    if(crypto_sign_keypair(pk, sk) != 0) {
        hal_send_str("Keypair generation failed!\n");
        return -1;
    }
    hal_send_str("✓ Keypair generation successful\n");

    // Sign message using internal function for deterministic results
    if(crypto_sign_signature_internal(sig, &siglen, message, 53, ctx, 0, rnd, sk) != 0) {
        hal_send_str("Signature generation failed!\n");
        return -1;
    }
    hal_send_str("✓ Signature generation successful\n");

    // Verify signature using internal function
    if(crypto_sign_verify_internal(sig, siglen, message, 53, ctx, 0, pk) != 0) {
        hal_send_str("Signature verification failed!\n");
        return -1;
    }
    hal_send_str("✓ Signature verification successful\n");

    hal_send_str("✓ ML-DSA functional test PASSED\n");
    return 0;
}

static void run_speed(void)
{
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];
    uint8_t message[53];
    uint8_t rnd[RNDBYTES];
    uint8_t ctx[1] = {0};
    size_t siglen;
    uint64_t cycles;
    char cycles_str[64];
    poly p;
    int i;

    hal_send_str("\n=== Benchmarks ===\n");

    // Setup test data
    memset(message, 0, 53);
    for(i = 0; i < RNDBYTES; i++) {
        rnd[i] = i;
    }

    // Initialize polynomial for NTT benchmark
    for(i = 0; i < N; i++) {
        p.coeffs[i] = i % Q;
    }

    // Keypair generation benchmark
    cycles = hal_get_time();
    crypto_sign_keypair(pk, sk);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for keypair generation: ");
#ifdef MPS2_AN386
    (void)cycles;
    sprintf(cycles_str, "[cycle counts not meaningful in qemu emulation]\n");
#else
    sprintf(cycles_str, "%llu\n", cycles);
#endif
    hal_send_str(cycles_str);

    // Signature generation benchmark (using generated keypair)
    cycles = hal_get_time();
    crypto_sign_signature_internal(sig, &siglen, message, 53, ctx, 0, rnd, sk);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for signature generation: ");
#ifdef MPS2_AN386
    (void)cycles;
    sprintf(cycles_str, "[cycle counts not meaningful in qemu emulation]\n");
#else
    sprintf(cycles_str, "%llu\n", cycles);
#endif
    hal_send_str(cycles_str);

    // Signature verification benchmark (using valid signature from above)
    cycles = hal_get_time();
    crypto_sign_verify_internal(sig, siglen, message, 53, ctx, 0, pk);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for signature verification: ");
#ifdef MPS2_AN386
    (void)cycles;
    sprintf(cycles_str, "[cycle counts not meaningful in qemu emulation]\n");
#else
    sprintf(cycles_str, "%llu\n", cycles);
#endif
    hal_send_str(cycles_str);

    // poly_ntt benchmark
    cycles = hal_get_time();
    poly_ntt(&p);
    cycles = hal_get_time() - cycles;
    hal_send_str("cycles for poly_ntt: ");
#ifdef MPS2_AN386
    (void)cycles;
    sprintf(cycles_str, "[cycle counts not meaningful in qemu emulation]\n");
#else
    sprintf(cycles_str, "%llu\n", cycles);
#endif
    hal_send_str(cycles_str);

    hal_send_str("Benchmarks completed!\n");
}

static void run_stack(void)
{
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];
    uint8_t message[53];
    uint8_t rnd[RNDBYTES];
    uint8_t ctx[1] = {0};
    size_t siglen;
    size_t stack_usage;
    char outstr[128];

    hal_send_str("\n=== Stack Usage Measurements ===\n");

    // Setup test data
    randombytes(message, 53);
    randombytes(rnd, RNDBYTES);

    // Measure stack usage for keypair generation
    hal_send_str("Measuring keypair generation stack usage...\n");
    hal_spraystack();
    crypto_sign_keypair(pk, sk);
    stack_usage = hal_checkstack();
    sprintf(outstr, "stack usage for keypair generation: %zu bytes", stack_usage);
    hal_send_str(outstr);

    // Measure stack usage for signature generation
    hal_send_str("Measuring signature generation stack usage...\n");
    hal_spraystack();
    crypto_sign_signature_internal(sig, &siglen, message, 53, ctx, 0, rnd, sk);
    stack_usage = hal_checkstack();
    sprintf(outstr, "stack usage for signature generation: %zu bytes", stack_usage);
    hal_send_str(outstr);

    // Measure stack usage for signature verification
    hal_send_str("Measuring signature verification stack usage...\n");
    hal_spraystack();
    crypto_sign_verify_internal(sig, siglen, message, 53, ctx, 0, pk);
    stack_usage = hal_checkstack();
    sprintf(outstr, "stack usage for signature verification: %zu bytes", stack_usage);
    hal_send_str(outstr);

    hal_send_str("Stack measurements completed!\n");
}

int main(void)
{
    hal_setup(CLOCK_BENCHMARK);

    // First test: verify keypair generation test vectors
    int test_result = test_keygen_vector();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }

    // Second test: verify signature generation test vectors
    test_result = test_signature_vector();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }

    // Third test: valid signature verification test vector
    test_result = test_valid_signature_verification();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }

    // Fourth test: invalid signature verification test vector
    test_result = test_invalid_signature_verification();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }

    // Fifth test: functional signature and verification
    test_result = test_functional();
    if(test_result != 0) {
        hal_send_str("\n*** TEST FAILED ***\n");
        return -1;
    }

    run_speed();
    run_stack();

    hal_send_str("\n*** ALL GOOD ***\n");
    return 0;
}