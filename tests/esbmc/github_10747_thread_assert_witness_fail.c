#include <assert.h>
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
  pthread_join(id, 0);
  assert(g == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10747_thread_assert_witness_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^    format_version: "2\.4"$
// EXPECT: (?s)type: "function_enter"(?:(?!waypoint:).)*line: 12\s+column: 30
// EXPECT: (?s)\A(?!.*thread_id)
// Vereda: verdict none
