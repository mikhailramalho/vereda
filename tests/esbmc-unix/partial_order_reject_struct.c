#include <pthread.h>
#include <assert.h>
struct s
{
  int f;
} g;
void *t(void *arg)
{
  g.f = 1;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  assert(g.f == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_struct
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a global that is not a scalar or an array of scalars yet, at file .*partial_order_reject_struct\.c line +16 column 3 function main$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
