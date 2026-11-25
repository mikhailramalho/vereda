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
// Needs: --sv-comp --state-hashing --add-symex-value-sets --smt-symex-guard --cswitch-skip-readonly-globals --data-races-check-only --no-assertions
// RUN: --32 --sv-comp --no-div-by-zero-check --force-malloc-success --state-hashing --add-symex-value-sets --smt-symex-guard --cswitch-skip-readonly-globals --no-pointer-check --no-bounds-check --data-races-check-only --no-assertions --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*data race.*$
// Vereda: verdict none
