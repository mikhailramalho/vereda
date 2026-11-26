/* local escapes via a store THROUGH a pointer: *pp = &i */
#include <pthread.h>
int *g;
void *t_fun(void *a) { (*g)++; return 0; }      /* RACE */
int main(void) { pthread_t id; int i = 0; int **pp = &g; *pp = &i;
  pthread_create(&id, 0, t_fun, 0);
  i++;                                          /* RACE */
  pthread_join(id, 0); return 0; }
// Imported from ESBMC regression/esbmc-unix/github_4424_escape_thru_deref
// Needs: --sv-comp --data-races-check-only
// RUN: -m32 --sv-comp --no-div-by-zero-check --force-malloc-success --no-pointer-check --no-bounds-check --data-races-check-only --no-assertions --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*data race.*$
// MODE: FUTURE
// Vereda: verdict none
