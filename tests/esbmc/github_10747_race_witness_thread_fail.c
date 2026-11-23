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
// Imported from ESBMC regression/esbmc/github_10747_race_witness_thread_fail
// Needs: --data-races-check --no-assertions --witness-output-yaml -
// RUN: --data-races-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: format_version: "2\.4"
// EXPECT: (?s)type: "target"(?:(?!waypoint:).)*thread_id: any\s+- waypoint:\s+type: "target"(?:(?!waypoint:).)*thread_id: any
// Vereda: verdict none
