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

struct priv shared;

void *worker(void *arg)
{
  shared.data = arg;
  return 0;
}

int main(void)
{
  struct dev *d = malloc(sizeof(struct dev));
  d->p = &shared;
  pthread_t id;
  pthread_create(&id, 0, worker, d);
  void *x = d->p->data;
  pthread_join(id, 0);
  return x != 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6596_uninit_ptr_race_fail
// Needs: --data-races-check-only
// RUN: -m32 --ub-pointer-check=false --data-races-check-only --no-assertions
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: R/W data race on
// MODE: FUTURE
// Vereda: verdict none
