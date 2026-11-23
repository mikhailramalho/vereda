#include <stdio.h>

#define test_type double
#define test_length sizeof(test_type)*4

typedef test_type v4si __attribute__((__vector_size__(test_length)));
v4si vsi = (v4si){0, 1, 2, 3};

// Should Initialize Correctly
int main() {
    vsi = vsi * 1.5;
   for(int i = 0; i < 4; i++)
      __VERIFIER_assert(vsi[i] == i*1.5); // The vector should be initialized correctly
   return 0;
}
// Imported from ESBMC regression/extensions/vector_gcc_literal_operator_float_mul_no_simplify_true
// Needs: --goto-unwind
// RUN: --goto-unwind --no-simplify
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
