union U { unsigned short a[3]; };
union U u[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&u[i] + 4;
  *(unsigned int *)p = 0x00020001u;
  __VERIFIER_assert(u[i + 1].a[0] == 0); // expect FAIL
  return 0;
}
// Imported from ESBMC regression/esbmc/array_union_spanning_fail
// EXPECT: ^VERIFICATION FAILED$
