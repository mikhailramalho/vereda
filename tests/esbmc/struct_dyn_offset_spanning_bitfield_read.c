struct S { unsigned int a : 4; unsigned int b : 28; unsigned int c; };
struct S s[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  s[i].a = 1; s[i].b = 3;
  unsigned int v = *(unsigned int *)&s[i];
  __VERIFIER_assert(v == 0x31u); // read
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_dyn_offset_spanning_bitfield_read
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
