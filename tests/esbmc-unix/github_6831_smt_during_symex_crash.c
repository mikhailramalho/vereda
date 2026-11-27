#include <pthread.h>
#include <assert.h>

int __VERIFIER_nondet_int();

int g = 0, h = 0;
int *a[2] = {&g, &h};
int **ap;

// Segfaulted under --smt-during-symex: an array-of-pointers select projects
// tuple elements that pop_ctx had already destroyed (issue #6831, #7049).
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
  g = 2;
  int seen = g;
  pthread_join(t, 0);
  assert(seen == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_smt_during_symex_crash
// RUN: --solver bitwuzla --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
