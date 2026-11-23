#include <assert.h>
#include <stddef.h>
extern void __VERIFIER_assume(int);

int main(){
  __VERIFIER_assume(NULL != "a");
  char *str0 = (char *) 0xFFFFFFFFFFFFFFFF;
  char *str1 = "x";
  assert(str0 <= str1);
}
// Imported from ESBMC regression/esbmc/github_1537-assume
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bassertion str0 <= str1$
// EXPECT: \bSAME-OBJECT\(str0, str1\)$
// Vereda: unmet EXPECT: \bassertion str0 <= str1$
// MODE: FUTURE
// Vereda: verdict FAILED
