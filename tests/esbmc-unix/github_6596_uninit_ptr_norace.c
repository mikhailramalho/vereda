#include <pthread.h>
#include <stdlib.h>

struct priv
{
  void *data;
};

struct dev
{
  struct priv *p;
};

void *worker(void *arg)
{
  return 0;
}

int main(void)
{
  struct dev *d = malloc(sizeof(struct dev));
  pthread_t id;
  pthread_create(&id, 0, worker, 0);
  void *x = d->p->data;
  pthread_join(id, 0);
  return x != 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6596_uninit_ptr_norace
// Needs: --data-races-check-only
// RUN: -m32 --ub-pointer-check=false --data-races-check-only --no-assertions --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
