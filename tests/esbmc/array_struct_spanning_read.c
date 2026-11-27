struct S { unsigned short a, b, c; };
struct S s[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  s[i].c = 0x1111; s[i + 1].a = 0x2222;
  unsigned char *p = (unsigned char *)&s[i] + 4;
  unsigned int v = *(unsigned int *)p;
  __VERIFIER_assert(v == 0x22221111u); // read spans
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_read
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
