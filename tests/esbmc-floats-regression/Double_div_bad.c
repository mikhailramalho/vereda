extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }

// Replace 1.6 to 2.5 to verification successful
#define X 1.6

int main()
{
  double x = 1.0;
  double x1 = x/X;

  while(x1 != x)
  {
    x = x1;
    x1 = x/X;
  }

  __VERIFIER_assert(x == 0);
  return 0;
}

// Imported from ESBMC regression/floats-regression/Double_div_bad
// Needs: --interval-analysis
// RUN: --floatbv --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
