struct S { unsigned short a, b, c; };
struct S s[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&s[i] + 5;
  *(unsigned int *)p = 0x03020100u;   /* bytes 5..8: s[i].c hi=00, s[i+1].a=0x0201, s[i+1].b lo=03 */
  __VERIFIER_assert(s[i + 1].a == 0); // expect FAIL
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_unaligned_fail
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// Vereda: KNOWNBUG test passed unexpectedly; promote it to CORE
// MODE: CORE
