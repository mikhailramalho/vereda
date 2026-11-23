/* The per-property report, and the CWE line its rows carry, must appear under
   --k-induction-parallel too: the base-case child holds the verdicts and is
   the process whose result decides the run. */
int main(void)
{
  int *p = 0;
  *p = 42;
  return 0;
}
// Imported from ESBMC regression/k-induction-parallel/property_report_bug
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^  FAILED +\[main\.null-pointer-dereference\.1\]  line 7  dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
