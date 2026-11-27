// #10749: the target runs in the spawned thread, so it needs thread_id 1.
#include <assert.h>
#include <pthread.h>
int g;
void *t(void *arg)
{
  for (int k = 0; k < 2; k++)
    g++;
  assert(g != 2);
  return 0;
}
int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10749_thread_target_witness_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^    format_version: "2\.4"$
// EXPECT: (?s)type: "function_enter"\s+action: "follow"\s+location:\s+file_name: "[^"]*github_10749_thread_target_witness_fail\.c"\s+line: 15\s+column: 30\s+function: "main"\s+- segment:
// EXPECT: (?s)type: "target"(?:(?!waypoint:).)*function: "t"\s+thread_id: 1
// Vereda: verdict none
