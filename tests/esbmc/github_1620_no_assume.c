#include <assert.h>
#include <stddef.h>
extern void __VERIFIER_assume(int);

int main(){
  const char* x4 = "";
  const char* __cil_tmp = "";
  const char* invalid_char_pt = (char *) 0x55a8a2e6b007;
  assert(invalid_char_pt != x4);
}
// Imported from ESBMC regression/esbmc/github_1620_no_assume
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
