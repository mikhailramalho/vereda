struct S { unsigned short a : 4, b : 12; unsigned short c; unsigned short d; };
struct S s[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&s[i] + 4;
  *(unsigned int *)p = 0x22221111u;
  __VERIFIER_assert(s[i].d == 0x1111); // d
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_bitfield_tail
// EXPECT: ^VERIFICATION SUCCESSFUL$
