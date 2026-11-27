/* local escapes through a helper that stores it into a global */
#include <pthread.h>
int *g;
void set_ptr(int *q) { g = q; }
void *t_fun(void *a) { (*g)++; return 0; }      /* RACE */
int main(void) { pthread_t id; int i = 0; set_ptr(&i);
  pthread_create(&id, 0, t_fun, 0);
  i++;                                          /* RACE */
  pthread_join(id, 0); return 0; }
// Imported from ESBMC regression/esbmc-unix/github_4424_escape_via_callee
// Needs: --sv-comp --data-races-check-only
// RUN: -m32 --sv-comp --ub-div-by-zero-check=false --force-malloc-success --ub-pointer-check=false --data-races-check-only --no-assertions --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*data race.*$
// MODE: FUTURE
// Vereda: verdict none
