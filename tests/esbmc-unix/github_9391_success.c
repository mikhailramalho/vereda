#include <assert.h>
#include <unistd.h>

/* #9391: getopt() returns the option character it finds; with at most two
 * options read, each flag is set by its own case. */
int main(int argc, char **argv)
{
  int aflag = 0, bflag = 0, c;
  for (int i = 0; i < 2 && (c = getopt(argc, argv, "ab")) != -1; i++)
    switch (c)
    {
    case 'a':
      aflag = 1;
      break;
    case 'b':
      bflag = 1;
      break;
    default:
      return 1;
    }
  assert(aflag + bflag <= 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9391_success
// RUN: --unwind 3 --multi-property
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
