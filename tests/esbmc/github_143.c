#include <pthread.h>
typedef struct
{
  int a[20];
  int b;
  int amount;
} c;
c d;
e(c *f, g)
{
  f->a[f->b] = g;
  f->amount++;
  if(f->b)
    ;
}
k(f)
{
  int i;
  e(&d, i);
}
main()
{
  pthread_t j;
  pthread_create(j, NULL, k, &d);
}
// Imported from ESBMC regression/esbmc/github_143
// RUN: --solver z3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
