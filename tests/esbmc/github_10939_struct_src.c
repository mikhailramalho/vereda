/* A union read over a struct of scalars at a dynamic (but zero) offset, as
 * Rust's ptr::swap_chunk reads a MaybeUninit<[u8; 4]> over a CannotDrop<char>
 * (#10939). */
#include <assert.h>
#include <stddef.h>
struct cannot_drop { unsigned int c; };
union chunk { unsigned char bytes[4]; };
size_t __VERIFIER_nondet_size(void);
int main(void)
{
  struct cannot_drop x = {0x11223344u};
  size_t off = __VERIFIER_nondet_size();
  __VERIFIER_assume(off == 0);
  union chunk a = *(union chunk *)((unsigned char *)&x + off);
  assert(a.bytes[0] == 0x44);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_struct_src
// EXPECT: ^VERIFICATION SUCCESSFUL$
