// Reproducer for esbmc/esbmc#4305: CTest generator must handle a
// nondeterministic int cast to int64_t alongside a nondet double whose
// satisfying assignment may be +/-INFINITY or NaN, without producing
// non-portable float literals in the generated test cases.
#include <stdint.h>

int __VERIFIER_nondet_int(void);
double __VERIFIER_nondet_double(void);

int main(void)
{
  int64_t timestamp_ns = (int64_t)__VERIFIER_nondet_int();
  double timestamp_s = (double)timestamp_ns / 1.0e9;

  double last_output_s = __VERIFIER_nondet_double();

  if (last_output_s > 0.0)
  {
    double elapsed = timestamp_s - last_output_s;
    if (elapsed >= 0.1)
      return 1;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_int64
// Needs: --condition-coverage --generate-ctest-testcase
// RUN: --condition-coverage --generate-ctest-testcase --unwind 1 --no-unwinding-assertions --force-malloc-success
// MODE: FUTURE
// EXPECT: ^Generated 4 CTest test case\(s\) with CMakeLists\.txt
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
