// process_goto_program treats --data-races-check-only, like --data-races-check,
// as a request to add race assertions. Those are injected during symex and so
// are not in all_claims, while --dead-code-check forces a SUCCESSFUL verdict —
// the W/W race on g below would be silently masked (issue #4495). The
// combination must be rejected up front.
#include <pthread.h>
int g;
void *t(void *p)
{
  g = g + 1;
  return 0;
}
int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, t, 0);
  pthread_create(&b, 0, t, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_race_only
// Needs: --dead-code-check --data-races-check-only
// RUN: --dead-code-check --data-races-check-only --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --data-races-check-only$
// Vereda: verdict none
