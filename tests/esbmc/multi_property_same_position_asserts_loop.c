/* Instances of a violated claim after its ASSERT became a SKIP must keep its
   row (esbmc/esbmc#7900). */
int a[4];
int main()
{
  int s = 0;
  for (unsigned k = 0; k < 3; ++k)
  {
    unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
    __VERIFIER_assume(i < 4);
    if (k == 1)
      __VERIFIER_assume(j < 4);
    s += a[i] + a[j];
  }
  return s;
}
// Imported from ESBMC regression/esbmc/multi_property_same_position_asserts_loop
// RUN: --unwind 4 --multi-property
// EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+13\s+array bounds violated: array `a' upper bound \[\([^)]*\)i < 4\]$
// EXPECT: ^\s+FAILED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+13\s+array bounds violated: array `a' upper bound \[\([^)]*\)j < 4\]$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+13\s+array bounds violated: array `a' upper bound \[\([^)]*\)i < 4\]$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
