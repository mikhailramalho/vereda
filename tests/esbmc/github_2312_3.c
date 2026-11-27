#include <stdio.h>
#include <assert.h>


int main(int argc, char *argv[])
{
    if(argc>0)
      assert(argv[1] != NULL);

    printf("Argument 1: %s\n", argv[1]);

    return 0;
}
// Imported from ESBMC regression/esbmc/github_2312_3
// EXPECT: ^VERIFICATION FAILED$
