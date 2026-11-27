/* A union read over a struct of scalars at offset 1 runs past the object. */
#include <stddef.h>
struct cannot_drop { unsigned int c; };
union chunk { unsigned char bytes[4]; };
size_t __VERIFIER_nondet_size(void);
int main(void)
{
  struct cannot_drop x = {0x11223344u};
  size_t off = __VERIFIER_nondet_size();
  __VERIFIER_assume(off == 1);
  union chunk a = *(union chunk *)((unsigned char *)&x + off);
  return a.bytes[0];
}
// Imported from ESBMC regression/esbmc/github_10939_struct_src_offset_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[main\.assertion\.[0-9]+\] +line +[0-9]+ +dereference failure: .*got illegal offset
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
