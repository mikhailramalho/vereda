/* C11 6.3.1.4p1: if the integral part of a floating value cannot be represented
   by the destination integer type, the behaviour is undefined. This went
   unchecked, so ESBMC reported SUCCESSFUL on undefined code (#7572). */
int main(void)
{
  double d = 1e300;
  long long x = (long long)d;
  (void)x;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7572_fail
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*floating-point conversion out of range.*$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
