#include <stdlib.h>
#include <limits.h>
#include <assert.h>

char __VERIFIER_nondet_char();

int main() {
  char *p, i, n;
  p = (char*)calloc(4, sizeof(char));
  if(p==NULL) {
    exit(1);
  }
  for(i = 0; i < 4; i++){
    *(p + i) = __VERIFIER_nondet_char();
  }
  p = (char*)realloc(p, 7 * sizeof(char));
  if(p==NULL) {
    exit(1);
  }
  for(i = 4; i < 7; i++) {
    *(p + i) = __VERIFIER_nondet_char();
  }
  for(i = 0; i < 7; i++) {
    assert(*(p+i) >= -128 && *(p+i) <=CHAR_MAX);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_151-1
// EXPECT: ^VERIFICATION SUCCESSFUL$
