#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  if(argc>1 && argv[100])
    assert(argv[100] != NULL); 
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2312_2
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
