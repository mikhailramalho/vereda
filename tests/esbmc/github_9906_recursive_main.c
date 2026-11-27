// #9906: the inner main ends with the jump pending for the outer main's setjmp
#include <setjmp.h>
#include <assert.h>
jmp_buf b;
int n;
int main(int argc, char **argv)
{
  if (n++ == 0)
  {
    int r = setjmp(b);
    if (r == 0)
      main(argc, argv);
    assert(r == 1);
    return 0;
  }
  longjmp(b, 1);
}
// Imported from ESBMC regression/esbmc/github_9906_recursive_main
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
