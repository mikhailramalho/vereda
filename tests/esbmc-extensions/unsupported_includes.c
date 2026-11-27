#include <stdatomic.h>
#include <pmmintrin.h>
#include <xmmintrin.h>
#include <x86intrin.h>
#include <immintrin.h>
#include <emmintrin.h>


void test_nonatomic_to_atomic(int n) {
    _Atomic int atomic_var = n; // Implicit conversion from int to _Atomic(int)
}

int main()
{

}
// Imported from ESBMC regression/extensions/unsupported_includes
// Needs: --dont-care-about-missing-extensions REQUIRES arch_x86
// RUN: --dont-care-about-missing-extensions --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
