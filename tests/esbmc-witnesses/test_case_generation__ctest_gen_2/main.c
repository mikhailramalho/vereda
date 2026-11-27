#include <assert.h>

extern _Bool __VERIFIER_nondet_bool(void);

int main(void) {
    _Bool flag = __VERIFIER_nondet_bool();

    if (flag) {
        return 0;
    }

    return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_2
// Needs: --branch-coverage --generate-ctest-testcase
// RUN: --branch-coverage --generate-ctest-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Generated 2 CTest test case\(s\) with CMakeLists\.txt
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
