#include <assert.h>
#include <pthread.h>

int g = 0, h = 0;
int *a[2] = {&h, &g};
int **ap;

void *writer(void *arg)
{
  (void)arg;
  **ap = 1;
  return 0;
}

int main(void)
{
  pthread_t t;
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  ap = &a[i];
  pthread_create(&t, 0, writer, 0);
  pthread_join(t, 0);
  g = 2;
  assert(g == 2 && (h == 0 || h == 1));
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/por_multi_target_pointer_write
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
