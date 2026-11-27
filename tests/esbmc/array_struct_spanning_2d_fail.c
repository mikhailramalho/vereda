struct S { unsigned short a, b, c; };
struct S s[2][3];
int main(void)
{
  unsigned char *p = (unsigned char *)&s[0][2] + 4;
  *(unsigned int *)p = 0x00020001u;
  __VERIFIER_assert(s[0][2].c == 1); // c
  __VERIFIER_assert(s[1][0].a == 0); // next row a: expect FAIL
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_2d_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
