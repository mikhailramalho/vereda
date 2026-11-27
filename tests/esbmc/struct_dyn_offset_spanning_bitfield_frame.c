struct S { unsigned int a : 4; unsigned int b : 28; unsigned int c; };
struct S s[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  s[i].b = 0x1234567; s[i].c = 9;
  *(unsigned short *)&s[i] = 0xABCDu;   /* a = 0xD, b low 12 bits = 0xABC */
  __VERIFIER_assert(s[i].a == 0xD); // a
  __VERIFIER_assert(s[i].b == ((0x1234567u & ~0xFFFu) | 0xABCu)); // b keeps high bits
  __VERIFIER_assert(s[i].c == 9); // c untouched
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_dyn_offset_spanning_bitfield_frame
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
