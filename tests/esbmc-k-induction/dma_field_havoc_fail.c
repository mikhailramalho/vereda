#include <stdint.h>

#define WORDS 4u
#define PAGES 3u

typedef struct
{
  uint32_t cfg_count;
  uint8_t mode;
  uint64_t timestamp;
  uint32_t hdr[PAGES * WORDS];
  uint32_t table[4];
} state_t;

static state_t state;
unsigned __VERIFIER_nondet_uint(void);

static void dma_read(uint32_t *dest, unsigned bytes)
{
  uint32_t tmp[WORDS];
  __ESBMC_init_object(tmp);
  #pragma unroll ((WORDS) + 1)
  for (unsigned i = 0; i < bytes / sizeof(uint32_t); ++i)
    dest[i] = tmp[i];
}

static void load_header(state_t *s, unsigned bytes)
{
  for (unsigned page = 0; page < PAGES; ++page)
  {
    unsigned chunk = bytes > WORDS * 4u ? WORDS * 4u : bytes;
    dma_read(&s->hdr[page * WORDS], chunk);
    bytes -= chunk;
  }
}

static void save_once(state_t *s)
{
  unsigned size = __VERIFIER_nondet_uint();
  __VERIFIER_assume(size <= PAGES * WORDS * 4u);
  load_header(s, size);
  s->table[s->cfg_count] = s->hdr[0];
}

int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 1000000u);
  __ESBMC_init_object(&state);
  __VERIFIER_assume(state.cfg_count <= 4);
  for (unsigned i = 0; i < n; ++i)
    save_once(&state);
}
// Imported from ESBMC regression/k-induction/dma_field_havoc_fail
// Needs: __ESBMC_init_object
// Needs: --k-induction
// RUN: --k-induction --k-step 4 --max-k-step 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED.*Access to object out of bounds
