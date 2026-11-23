#include <assert.h>
#include <pthread.h>

int n;

void *worker(void *arg)
{
  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t id;
  pthread_create(&id, 0, worker, 0);
  pthread_join(id, 0);
  assert(n == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1583_argv_smt_symex_guard
// Needs: --smt-symex-guard
// RUN: --smt-symex-guard
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
