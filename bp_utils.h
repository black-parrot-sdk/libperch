#ifndef BP_UTILS_H
#define BP_UTILS_H

#include <stdint.h>
#include <bp_asm.h>
#include <bp_aviary.h>
#include <bp_config.h>
#include <bp_host.h>
#include <encoding.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t bp_get_hart(void);

void bp_barrier_end(volatile uint64_t *barrier_address, uint64_t total_num_cores);

// Prints the given nibble as a hex character. 0 <= nibble <= 0xf.
void bp_hprint(uint8_t nibble);

// Prints the provided ASCII character.
void bp_cprint(int ch);

// Prints the given value as a 16-nibble hex number, preceded by "0x".
void bp_hprint_uint64(uint64_t val);

void bp_print_string(char *str);

int bp_cget(void);

void bp_panic(char *message) __attribute__((noreturn));

void bp_finish(uint8_t code) __attribute__((noreturn));

uint32_t bp_param_get(char *addr);

#ifdef __cplusplus
}
#endif

#endif
