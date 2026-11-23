#include <stdlib.h>
int func(){
  char *memoryArea = malloc(10); //return_value_malloc$1
  char *newArea = malloc(10); //return_value_malloc$2
  memoryArea = newArea; 
  free(memoryArea);
}
void main(){
  func();
  char *test = malloc(20); //return_value_malloc$1
  free(test);
}
// Imported from ESBMC regression/esbmc/00_memory_leak_01_0
// Needs: --memory-leak-check
// RUN: --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
