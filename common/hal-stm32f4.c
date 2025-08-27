#include "hal.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rng.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/systick.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

/* 24 MHz */
const struct rcc_clock_scale benchmarkclock = {
  .pllm = 8, //VCOin = HSE / PLLM = 1 MHz
  .plln = 192, //VCOout = VCOin * PLLN = 192 MHz
  .pllp = 8, //PLLCLK = VCOout / PLLP = 24 MHz (low to have 0WS)
  .pllq = 4, //PLL48CLK = VCOout / PLLQ = 48 MHz (required for USB, RNG)
  .pllr = 0,
  .hpre = RCC_CFGR_HPRE_DIV_NONE,
  .ppre1 = RCC_CFGR_PPRE_DIV_2,
  .ppre2 = RCC_CFGR_PPRE_DIV_NONE,
  .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
  .voltage_scale = PWR_SCALE1,
  .flash_config = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_LATENCY_0WS,
  .ahb_frequency = 24000000,
  .apb1_frequency = 12000000,
  .apb2_frequency = 24000000,
};

static void clock_setup(const enum clock_mode clock)
{
  switch(clock)
  {
    case CLOCK_BENCHMARK:
      rcc_clock_setup_pll(&benchmarkclock);
      break;
    case CLOCK_FAST:
    default:
      rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
      break;
  }

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART2);
  rcc_periph_clock_enable(RCC_DMA1);
  rcc_periph_clock_enable(RCC_RNG);

  flash_prefetch_enable();
}

static void gpio_setup(void)
{
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);
}
static void usart_setup(int baud)
{
  usart_set_baudrate(USART2, baud);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  usart_enable(USART2);
}

static void systick_setup(void)
{
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_set_reload(16777215);
  systick_interrupt_enable();
  systick_counter_enable();
}

static void send_USART_str(const char* in)
{
  int i;
  for(i = 0; in[i] != 0; i++) {
    usart_send_blocking(USART2, *(unsigned char *)(in+i));
  }
  usart_send_blocking(USART2, '\n');
}
void hal_setup(const enum clock_mode clock)
{
  clock_setup(clock);
  gpio_setup();
  usart_setup(115200);
  systick_setup();
  rng_enable();
}
void hal_send_str(const char* in)
{
  send_USART_str(in);
}


static volatile unsigned long long overflowcnt = 0;
void sys_tick_handler(void)
{
  ++overflowcnt;
}
uint64_t hal_get_time()
{
  while (true) {
    unsigned long long before = overflowcnt;
    unsigned long long result = (before + 1) * 16777216llu - systick_get_value();
    if (overflowcnt == before) {
      return result;
    }
  }
}




// Stack measurement implementation
extern char _end[];
extern char _estack[];
static char *heap_end = _end;
static const uint32_t stackpattern = 0xDEADBEEFlu;
static void* last_sp = NULL;

void hal_spraystack(void) {
    char* _heap_end = heap_end;
    asm volatile ("mov %0, sp\n"
                  ".L%=:\n\t"
                  "str %2, [%1], #4\n\t"
                  "cmp %1, %0\n\t"
                  "blt .L%=\n\t"
                  : "+r" (last_sp), "+r" (_heap_end) : "r" (stackpattern) : "cc", "memory");
}

size_t hal_checkstack(void) {
    size_t result = 0;
    asm volatile("sub %0, %1, %2\n"
                 ".L%=:\n\t"
                 "ldr ip, [%2], #4\n\t"
                 "cmp ip, %3\n\t"
                 "ite eq\n\t"
                 "subeq %0, #4\n\t"
                 "bne .LE%=\n\t"
                 "cmp %2, %1\n\t"
                 "blt .L%=\n\t"
                 ".LE%=:\n"
                 : "+r"(result) : "r" (last_sp), "r" (heap_end), "r" (stackpattern) : "ip", "cc");
    return result;
}

size_t hal_get_stack_size(void)
{
  register char* cur_stack;
	asm volatile ("mov %0, sp" : "=r" (cur_stack));
  return cur_stack - heap_end;
}

void* __wrap__sbrk(int incr) {
    char *current_heap_end = heap_end;
    heap_end += incr;
    return current_heap_end;
}

/* System call stubs - suppress warnings from newer ARM toolchains */

int __wrap__close(int fd) {
    (void)fd;
    errno = ENOSYS;
    return -1;
}

int __wrap__fstat(int fd, struct stat *buf) {
    (void)fd;
    (void)buf;
    errno = ENOSYS;
    return -1;
}

pid_t __wrap__getpid(void) {
    errno = ENOSYS;
    return -1;
}

int __wrap__isatty(int fd) {
    (void)fd;
    errno = ENOSYS;
    return 0;
}

int __wrap__kill(pid_t pid, int sig) {
    (void)pid;
    (void)sig;
    errno = ENOSYS;
    return -1;
}

off_t __wrap__lseek(int fd, off_t offset, int whence) {
    (void)fd;
    (void)offset;
    (void)whence;
    errno = ENOSYS;
    return -1;
}

ssize_t __wrap__read(int fd, void *buf, size_t count) {
    (void)fd;
    (void)buf;
    (void)count;
    errno = ENOSYS;
    return -1;
}

ssize_t __wrap__write(int fd, const void *buf, size_t count) {
    (void)fd;
    (void)buf;
    (void)count;
    errno = ENOSYS;
    return -1;
}
