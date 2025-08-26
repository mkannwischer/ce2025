#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "group.h"
#include "smult.h"
#include "hal.h"


unsigned char sk0[32] = {0xb1, 0x7a, 0xa0, 0x76, 0x93, 0xd7, 0x8d, 0x70, 0xfb, 0x44, 0x3a, 0x5b, 0xf1, 0xc6, 0x90, 0xe2,
                         0xc3, 0x79, 0x39, 0x6f, 0x56, 0xac, 0xc5, 0x5f, 0xb5, 0xfc, 0x1c, 0xc5, 0x58, 0xa2, 0xd9, 0x85};

unsigned char sk1[32] = {0xba, 0xdb, 0xc5, 0x8f, 0xc7, 0x97, 0x18, 0xc4, 0x78, 0x32, 0x13, 0x0a, 0x94, 0x2c, 0x80, 0xdb,
                         0x77, 0x84, 0x34, 0xdc, 0x04, 0xce, 0x19, 0x16, 0xda, 0xe4, 0x16, 0x36, 0x06, 0xca, 0xdd, 0x30};


unsigned char cmppk0[32] = {0x54, 0xba, 0x6e, 0xf0, 0x36, 0xa4, 0x11, 0xc9, 0xa5, 0x29, 0x4d, 0xb9, 0xbe, 0x38, 0x9f, 0xbc,
                            0x2c, 0xe1, 0x90, 0xa1, 0xf2, 0x20, 0x09, 0xd1, 0xd7, 0x8f, 0x9b, 0x56, 0xc0, 0xa2, 0x14, 0x62};

unsigned char cmppk1[32] = {0x82, 0xe3, 0x9b, 0x97, 0xd6, 0x73, 0xb7, 0x72, 0xdf, 0x34, 0x79, 0xbf, 0xed, 0x94, 0x31, 0x7f,
                            0x59, 0x83, 0x48, 0xd1, 0xa5, 0x29, 0x14, 0xfd, 0xf7, 0x67, 0x7c, 0x17, 0x46, 0xd0, 0x59, 0x6a};

unsigned char cmpss[32]  = {0xfe, 0xb3, 0xdd, 0x58, 0x73, 0x4b, 0x42, 0xc8, 0x86, 0x0d, 0x2b, 0xb7, 0x08, 0xc0, 0xae, 0x14,
                            0x7a, 0x21, 0xdf, 0x42, 0xf8, 0xc9, 0xaf, 0x4e, 0x3c, 0xc4, 0xbe, 0x8c, 0x56, 0xfc, 0x88, 0x3d};


static int run_tests(void)
{
  int i;
  unsigned char pk0[GROUP_GE_PACKEDBYTES];
  unsigned char pk1[GROUP_GE_PACKEDBYTES];
  unsigned char ss0[GROUP_GE_PACKEDBYTES];
  unsigned char ss1[GROUP_GE_PACKEDBYTES];

  hal_send_str("==================\n");
  hal_send_str("Running test...\n\n");

  crypto_scalarmult_base(pk0, sk0);
  crypto_scalarmult_base(pk1, sk1);

  crypto_scalarmult(ss0, sk0, pk1);
  crypto_scalarmult(ss1, sk1, pk0);

  for(i=0;i<32;i++)
  {
    if(ss0[i] != ss1[i])
    {
      hal_send_str("ERROR: shared secrets don't match\n");
      return 1;
    }

    if(pk0[i] != cmppk0[i])
    {
      hal_send_str("ERROR: pk0 mismatch\n");
      return 1;
    }
    if(pk1[i] != cmppk1[i])
    {
      hal_send_str("ERROR: pk1 mismatch\n");
      return 1;
    }
    if(ss0[i] != cmpss[i])
    {
      hal_send_str("ERROR: shared secret mismatch\n");
      return 1;
    }
  }

  hal_send_str("Test successful!\n");
  return 0;
}

static void run_speed(void)
{
  unsigned char pk[32], ss[32];
  uint64_t cycles;
  char cycles_str[64];

  hal_send_str("\n=== Benchmarks ===\n");

  cycles = hal_get_time();
  crypto_scalarmult_base(pk, sk0);
  cycles = hal_get_time() - cycles;
  hal_send_str("cycles for crypto_scalarmult_base: ");
#ifdef MPS2_AN386
  (void)cycles;
  sprintf(cycles_str, "[cycle counts not meaningful in qemu emulation]\n");
#else
  sprintf(cycles_str, "%llu\n", (unsigned long long)cycles);
#endif
  hal_send_str(cycles_str);

  cycles = hal_get_time();
  crypto_scalarmult(ss, sk0, pk);
  cycles = hal_get_time() - cycles;
  hal_send_str("cycles for crypto_scalarmult: ");
#ifdef MPS2_AN386
  (void)cycles;
  sprintf(cycles_str, "[cycle counts not meaningful in qemu emulation]\n");
#else
  sprintf(cycles_str, "%llu\n", (unsigned long long)cycles);
#endif
  hal_send_str(cycles_str);
}

static void run_stack(void)
{
  unsigned char pk[32], ss[32];
  size_t stack_usage;
  char outstr[128];

  hal_send_str("\n=== Stack Usage Measurements ===\n");

  // Measure stack usage for crypto_scalarmult_base (public key generation)
  hal_send_str("Measuring crypto_scalarmult_base stack usage...\n");
  hal_spraystack();
  crypto_scalarmult_base(pk, sk0);
  stack_usage = hal_checkstack();
  sprintf(outstr, "stack usage for crypto_scalarmult_base: %zu bytes", stack_usage);
  hal_send_str(outstr);

  // Measure stack usage for crypto_scalarmult (shared secret computation)
  hal_send_str("Measuring crypto_scalarmult stack usage...\n");
  hal_spraystack();
  crypto_scalarmult(ss, sk0, pk);
  stack_usage = hal_checkstack();
  sprintf(outstr, "stack usage for crypto_scalarmult: %zu bytes", stack_usage);
  hal_send_str(outstr);

  hal_send_str("Stack measurements completed!\n");
}

int main(void)
{
  hal_setup(CLOCK_BENCHMARK);

  int test_result = run_tests();

  run_speed();
  run_stack();

  if(test_result != 0) {
    hal_send_str("\n*** TEST FAILED ***\n");
    return -1;
  }

  hal_send_str("\n*** ALL GOOD ***\n");
  return 0;
}

