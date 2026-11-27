// A store at a symbolic index after the loop leaves the read unreduced, so
// the solver gets the whole summarised array.
unsigned __VERIFIER_nondet_uint(void);
int a[100];
int main()
{
  for (unsigned i = 0; i < 100; i++)
    a[i] = i;
  unsigned c = __VERIFIER_nondet_uint();
  __VERIFIER_assume(c < 100);
  a[c] = -1;
  unsigned y = __VERIFIER_nondet_uint();
  __VERIFIER_assume(y < 100);
  __VERIFIER_assert(a[y] == y); // only a[c] changed
}
// Imported from ESBMC regression/esbmc/array_loop_summary_update_fail
// EXPECT: ^VERIFICATION FAILED$
