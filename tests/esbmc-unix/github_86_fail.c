/* #86: --schedule never ran the memory-leak check on a schedule that took a
 * context switch, so this leak was reported as VERIFICATION SUCCESSFUL. */
#include <pthread.h>
#include <stdlib.h>

void *worker(void *arg)
{
  return 0;
}

int main()
{
  pthread_t id;
  pthread_create(&id, 0, worker, 0);
  int *p = malloc(sizeof(int));
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_86_fail
// Needs: --memory-leak-check
// RUN: --memory-leak-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// Vereda: verdict none
