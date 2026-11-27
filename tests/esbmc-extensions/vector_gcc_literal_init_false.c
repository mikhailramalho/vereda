#include <stdio.h>

// Only sizes that are positive power-of-two multiples of the base type size are currently allowed. 
typedef int v4si __attribute__((__vector_size__(1)));
v4si vsi;

int main() {
    
   return 0; 
}
// Imported from ESBMC regression/extensions/vector_gcc_literal_init_false
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
