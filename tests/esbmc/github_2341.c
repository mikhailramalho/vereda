#include <stdlib.h>
#include <assert.h>

int main() {
  int *ptr = NULL;
  int *temp = (int*)realloc(ptr, 10 * sizeof(int)); 

  if (temp != NULL) { 
      ptr = temp;
      ptr[0] = 42;
      ptr[9] = 99;
  }

  free(ptr);
  return 0;
} 
// Imported from ESBMC regression/esbmc/github_2341
// EXPECT: ^VERIFICATION SUCCESSFUL$
