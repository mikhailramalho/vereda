#include <stdio.h>
#include <stdlib.h>

void test() {
	
  int size = __VERIFIER_nondet_uint();
  
  if (size > 0) {
    
	int* array = (int*)(malloc(size * sizeof(int)));
        __VERIFIER_assume(array);
    
	int x = size - 1;
    int y = size - 1;
    
	int index = (x + y)/2;
    
	int c = array[index];
  }
}

int main(){
	test();	
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc28
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
