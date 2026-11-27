#include <mmintrin.h>

int main()
{
}
// Imported from ESBMC regression/esbmc-unix/unsupported_extensions
// Needs: REQUIRES arch_x86
// RUN: --function _mm_adds_pi8 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
