/* Two bound checks share a description and a position; each needs its own
   row (esbmc/esbmc#7900). */
int a[4];
int main()
{
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  int s = a[i] + a[j];
  return s;
}
// Imported from ESBMC regression/esbmc/multi_property_same_position_asserts
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+8\s+array bounds violated: array `a' upper bound \[\([^)]*\)i < 4\]$
// EXPECT: ^\s+FAILED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+8\s+array bounds violated: array `a' upper bound \[\([^)]*\)j < 4\]$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+8\s+array bounds violated: array `a' upper bound \[\([^)]*\)i < 4\]$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
