#include <iostream>
#include <cstdint>
#include <vector>

uint32_t rotate(uint32_t x, int bits);
uint32_t load(uint8_t *state, int pos);
void store(uint8_t *state, int pos, uint32_t x);
void gimli(uint8_t *state);
void gimli_SP_column(uint32_t *column);
void gimli_SPSP_column(uint32_t *column);
