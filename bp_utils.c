#include <stdint.h>
#include <bp_utils.h>

static uint64_t my_hart;

void bp_init(void) {
    __asm__ volatile("csrr %0, mhartid": "=r"(my_hart): :);
}

uint64_t bp_get_hart(void) {
    return my_hart;
}

void bp_barrier_end(volatile uint64_t *barrier_address, uint64_t total_num_cores) {
    uint64_t atomic_inc = 1;
    uint64_t atomic_result;
    __asm__ volatile("amoadd.d %0, %2, (%1)": "=r"(atomic_result) 
                                            : "r"(barrier_address), "r"(atomic_inc)
                                            :);
    
    while(*barrier_address < total_num_cores);
    bp_finish(0);
}

void bp_finish(uint8_t code) {
  uint64_t core_id = bp_get_hart();

  *(FINISH_BASE_ADDR+core_id*8) = code;
  __asm__ volatile("fence");
  while (1);
}

void bp_hprint(uint8_t nibble) {
  if (nibble <= 0x9) {
    bp_cprint('0' + nibble);
  } else if (nibble <= 0xf) {
    bp_cprint('a' + nibble - 0xa);
  }
}

void bp_cprint(int ch) {
  *(PUTCHAR_BASE_ADDR) = ch;
}

void bp_hprint_uint64(uint64_t val) {
  bp_cprint('0');
  bp_cprint('x');
  for (int i = 0; i < 16; i++) {
    uint8_t nibble = val >> 60;
    bp_hprint(nibble);
    val <<= 4;
  }
}

void bp_print_string(char *str) {
  while (*str) {
    bp_cprint(*str);
    str++;
  }
}

int bp_cget(void) {
  char c = *(GETCHAR_BASE_ADDR);
  // We don't have a empty fifo signal so just alias 255 to -1 for now, to simulate EOF
  if (c == 255) return -1;
  // Substitute CR with newline to make scanf work
  if (c == '\r') return '\n';
  
  return c;
}

void bp_panic(char *message) {
  if (message) {
    bp_print_string(message);
  }

  bp_finish(1);
}

uint32_t bp_param_get(char *addr) {
  return *(volatile uint32_t *) addr;
}

