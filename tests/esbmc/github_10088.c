struct S { unsigned short a, b, c; };
struct S s[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&s[i] + 4;
  *(unsigned int *)p = 0x00020001u;
  __VERIFIER_assert(s[i].c == 1); // c
  __VERIFIER_assert(s[i + 1].a == 2); // next a
  __VERIFIER_assert(s[i].a == 0 && s[i].b == 0 && s[i + 1].b == 0); // untouched
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10088
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
