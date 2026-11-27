#include <stdio.h>

#define test_type int
#define test_length sizeof(test_type)*4

typedef test_type v4si __attribute__((__vector_size__(test_length)));
v4si vsi = (v4si){1, 2, 3, 4};
v4si vsi2 = (v4si){4, 3, 2, 1};

// Should Initialize Correctly
int main() {
    vsi = vsi2 | vsi;
   for(int i = 0; i < 4; i++)
      __VERIFIER_assert(vsi[i] == 1); // The vector should be initialized correctly
   return 0; 
}
// Imported from ESBMC regression/extensions/vector_gcc_literal_operator_int_bitor_2_false
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
