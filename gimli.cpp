#include <iostream>
#include <cstdint>
#include <vector>

#include "gimli.hpp"

/*
  Here codes from Gimli reference implementation.
  Delete "static" and add function for SP and double SP operations.
*/
uint32_t rotate(uint32_t x,int bits)
{
  if (bits == 0) return x;
  return (x << bits) | (x >> (32 - bits));
}

uint32_t load(uint8_t *state,int pos)
{
  uint32_t result = state[4*pos+3];
  result <<= 8; result |= state[4*pos+2];
  result <<= 8; result |= state[4*pos+1];
  result <<= 8; result |= state[4*pos+0];
  return result;
}

void store(uint8_t *state,int pos,uint32_t x)
{
  state[4*pos+0] = x; x >>= 8;
  state[4*pos+1] = x; x >>= 8;
  state[4*pos+2] = x; x >>= 8;
  state[4*pos+3] = x;
}

void gimli(uint8_t *state)
{
  int round, column;
  uint32_t x, y, z;

  for (round = 24; round > 0; --round) {
    for (column = 0; column < 4; ++column) {
      x = rotate(load(state,    column), 24);
      y = rotate(load(state,4 + column),  9);
      z =        load(state,8 + column);

      store(state,8 + column,x ^ (z << 1) ^ ((y&z) << 2));
      store(state,4 + column,y ^ x        ^ ((x|z) << 1));
      store(state,    column,z ^ y        ^ ((x&y) << 3));
    }

    if ((round & 3) == 0) { // small swap: pattern s...s...s... etc.
      x = load(state,0);
      store(state,0,load(state,1));
      store(state,1,x);
      x = load(state,2);
      store(state,2,load(state,3));
      store(state,3,x);
    }
    if ((round & 3) == 2) { // big swap: pattern ..S...S...S. etc.
      x = load(state,0);
      store(state,0,load(state,2));
      store(state,2,x);
      x = load(state,1);
      store(state,1,load(state,3));
      store(state,3,x);
    }

    if ((round & 3) == 0) // add constant: pattern c...c...c... etc.
      store(state,0,load(state,0) ^ (0x9e377900 | round));
  }
}

/*
  SP function of Gimli permutation
  input is 32 * 3 unsigned int word
  */
void gimli_SP_column(uint32_t *column)
{
	uint32_t x, y, z;

	x = (rotate(column[0], 24)) & 0xffffffff;
	y = (rotate(column[1], 9)) & 0xffffffff;
	z = (column[2]) & 0xffffffff;

	column[2] = x ^ (z << 1) ^ ((y&z) << 2);
	column[1] = y ^ x ^ ((x | z) << 1);
	column[0] = z ^ y ^ ((x&y) << 3);
}

/*
	SP function applied twice
	input is 32 * 3 unsigned int word
*/
void gimli_SPSP_column(uint32_t *column)
{
	uint32_t x, y, z, ox, oy, oz;

	x = (rotate(column[0], 24))&0xffffffff;
	y = (rotate(column[1], 9)) & 0xffffffff;
	z = (column[2]) & 0xffffffff;

	oz = x ^ (z << 1) ^ ((y&z) << 2);
	oy = y ^ x ^ ((x | z) << 1);
	ox = z ^ y ^ ((x&y) << 3);

	x = (rotate(ox, 24)) & 0xffffffff;
	y = (rotate(oy, 9)) & 0xffffffff;
	z = (oz) & 0xffffffff;

	column[2] = x ^ (z << 1) ^ ((y&z) << 2);
	column[1] = y ^ x ^ ((x | z) << 1);
	column[0] = z ^ y ^ ((x&y) << 3);
}