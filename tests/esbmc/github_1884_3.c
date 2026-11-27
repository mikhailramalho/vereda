#include <assert.h>

int main()
{
  if( -((unsigned int)-1) < 0U){
    assert(0);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1884_3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
