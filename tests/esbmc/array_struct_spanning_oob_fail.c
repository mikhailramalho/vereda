struct S { unsigned short a, b, c; };
struct S s[3];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&s[i] + 4;
  *(unsigned int *)p = 1u;
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_oob_fail
// EXPECT: ^  dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
