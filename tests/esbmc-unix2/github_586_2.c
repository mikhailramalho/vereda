#include <pthread.h>

a;
*d, *e;
b()
{
  if (a)
    ;
}
c()
{
  do
    ;
  while (a == 0);
}
main()
{
  pthread_create(&d, 0, b, 0);
  pthread_create(&e, 0, c, 0);
}
// Imported from ESBMC regression/esbmc-unix2/github_586_2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
