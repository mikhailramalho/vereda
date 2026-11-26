#include <pthread.h>
int g;
void *t(void *arg)
{
  g = 1;
  return 0;
}
int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, t, 0);
  pthread_create(&b, 0, t, 0);
  pthread_join(a, 0);
  pthread_join(b, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9688_race_witness_symex_guard_fail
// Needs: --data-races-check-only --witness-output-yaml -
// RUN: --data-races-check-only --witness-output-yaml -
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)segment:\s+- waypoint:\s+type: "target"[^\n]*\n[^\n]*\n\s+location:[^\n]*\n[^\n]*\n\s+line: 5\b.*?- waypoint:\s+type: "target"[^\n]*\n[^\n]*\n\s+location:[^\n]*\n[^\n]*\n\s+line: 5\b
// MODE: FUTURE
// Vereda: verdict none
