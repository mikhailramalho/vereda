struct S { unsigned short a; unsigned short b : 4, c : 12; unsigned short d; };
struct S s[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&s[i] + 4;
  *(unsigned int *)p = 0x00020001u;
  __VERIFIER_assert(s[i].d == 1); // d
  __VERIFIER_assert(s[i + 1].a == 2); // next a
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_bitfield
// EXPECT: ^VERIFICATION SUCCESSFUL$
