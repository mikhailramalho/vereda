#include <assert.h>
extern int x;
int main(int argc, char **argv)
{
  if (__VERIFIER_nondet_int())
  {
    argc = 3;
  }
  else
  {
    argc = 2;
  }
  if (argc > 5)
  {
    x = 42;
  }
  assert(x == 42);
}
// Imported from ESBMC regression/esbmc/github_2569
// EXPECT: ^VERIFICATION FAILED$
