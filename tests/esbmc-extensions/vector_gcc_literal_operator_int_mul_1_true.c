#include <stdio.h>

#define test_type int
#define test_length sizeof(test_type)*4

typedef test_type v4si __attribute__((__vector_size__(test_length)));
v4si vsi = (v4si){1, 2, 3, 4};

// Should Initialize Correctly
int main() {
    vsi = 2 * vsi;
   for(int i = 0; i < 4; i++)
      __VERIFIER_assert(vsi[i] == (i+1)*2); // The vector should be initialized correctly
   return 0; 
}
// Imported from ESBMC regression/extensions/vector_gcc_literal_operator_int_mul_1_true
// EXPECT: ^VERIFICATION SUCCESSFUL$
