struct S { signed int a : 3; signed int b : 5; unsigned char c; unsigned short d; unsigned int e; };
struct S s;
int main(void)
{
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k == 0 || k == 4);
  *(unsigned int *)((char *)&s + k) = 0x0004F1FFu;
  __VERIFIER_assert(k != 0 || (s.a == -1 && s.b == -1 && s.c == 0xF1 && s.d == 4)); // all
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_dyn_offset_bitfield_signed_store
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
