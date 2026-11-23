#include <stdlib.h>
int func(){
  char *memoryArea = malloc(10); //return_value_malloc$1
  __VERIFIER_assume(memoryArea);
  char *newArea = malloc(10); //return_value_malloc$2
  __VERIFIER_assume(newArea);
  memoryArea = newArea; 
  free(memoryArea);
}
void main(){
  func();
  char *test = malloc(20); //return_value_malloc$1
  __VERIFIER_assume(test);
  free(test);
}
// Imported from ESBMC regression/esbmc/00_memory_leak_01
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
