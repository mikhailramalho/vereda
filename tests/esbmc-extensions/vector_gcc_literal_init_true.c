#include <stdio.h>

typedef int v4si __attribute__((__vector_size__(16)));
v4si vsi;

int main() {
    
   return 0; 
}
// Imported from ESBMC regression/extensions/vector_gcc_literal_init_true
// EXPECT: ^VERIFICATION SUCCESSFUL$
