#include <pthread.h>
int g;
void *t(void *arg)
{
  g = 1;
  return 0;
}
int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  g = 2;
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9688_race_witness_fail
// Needs: --data-race-check --no-assertions --witness-output-yaml -
// RUN: --data-race-check --no-assertions --witness-output-yaml -
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)segment:\s+- waypoint:\s+type: "target"[^\n]*\n[^\n]*\n\s+location:[^\n]*\n[^\n]*\n\s+line: (?:5|12)\b.*?- waypoint:\s+type: "target"[^\n]*\n[^\n]*\n\s+location:[^\n]*\n[^\n]*\n\s+line: (?:5|12)\b
// MODE: FUTURE
// Vereda: verdict none
