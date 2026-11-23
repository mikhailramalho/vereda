/* `b` is unconstrained, so the conversion below is undefined whenever its
   integral part does not fit an int, NaN included (C11 6.3.1.4p1). This
   expected SUCCESSFUL until #7572 added the check that catches it. */
int main()
{
  int a;
  double b=__VERIFIER_nondet_double();

  a = (int)b;
}
// Imported from ESBMC regression/esbmc/overflow_21_typecast
// RUN: --overflow-check
// EXPECT: ^.*floating-point conversion out of range.*$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^.*floating-point conversion out of range.*$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
