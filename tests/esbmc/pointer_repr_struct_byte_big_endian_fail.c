#include <assert.h>

struct entry
{
  int *key;
  int *value;
  char name[16];
};

// Under --big-endian a byte offset counts from the other end of the flattened
// struct. The byte written here is the first byte of key, so key changes.
int main()
{
  int k = 1, v = 2;
  struct entry e;
  e.key = &k;
  e.value = &v;
  unsigned char *bytes = (unsigned char *)&e;
  for (unsigned i = 0; i < 1; ++i)
    bytes[i] = 0xff;
  assert(e.key == &k);
}
// Imported from ESBMC regression/esbmc/pointer_repr_struct_byte_big_endian_fail
// Needs: --big-endian
// RUN: --unwind 2 --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
