// #10749: the registration sits on the ')' closing the call, past the
// parentheses inside the string argument and the (void) cast.
#include <assert.h>
#include <pthread.h>
void *t(void *arg)
{
  assert(*(const char *)arg != 'a');
  return 0;
}
int main(void)
{
  pthread_t id;
  (void)pthread_create(&id, 0, t, (void *)"a)\"(");
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10749_cast_spawn_witness_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^    format_version: "2\.4"$
// EXPECT: (?s)type: "function_enter"(?:(?!waypoint:).)*line: 13\s+column: 50
// EXPECT: (?s)type: "target"(?:(?!waypoint:).)*function: "t"\s+thread_id: 1
// Vereda: verdict none
