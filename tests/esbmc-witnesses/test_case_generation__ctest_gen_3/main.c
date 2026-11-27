#include <stdio.h>

extern char __VERIFIER_nondet_char(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
extern _Bool __VERIFIER_nondet_bool(void); 

int main(void) {
  char c = __VERIFIER_nondet_char();
  unsigned char uc = __VERIFIER_nondet_uchar();
  _Bool b = __VERIFIER_nondet_bool();

  if (b) {
    if (c >= 'A') {
      return 1;
    } else {
      return 2;
    }
  } else {
    if (uc > 128) {
      return 3;
    } else {
      return 0;
    }
  }
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_3
// Needs: --branch-coverage --generate-ctest-testcase
// RUN: --branch-coverage --generate-ctest-testcase
// MODE: FUTURE
// EXPECT: ^Generated 6 CTest test case\(s\) with CMakeLists\.txt
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
