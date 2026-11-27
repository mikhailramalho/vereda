// #10749: the nondet call in pthread_create's arguments returns before the spawn.
#include <pthread.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
void *t(void *a)
{
  assert((long)a != 5);
  return 0;
}
int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, t, (void *)(long)__VERIFIER_nondet_int());
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10749_spawn_arg_witness_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^    format_version: "2\.4"$
// EXPECT: (?s)type: "function_return"(?:(?!waypoint:).)*line: 13\s+column: 65\s+function: "main"\s+- segment:\s+- waypoint:\s+type: "function_enter"(?:(?!waypoint:).)*line: 13\s+column: 66\s+function: "main"\s+- segment:
// EXPECT: (?s)type: "target"(?:(?!waypoint:).)*function: "t"\s+thread_id: 1
// Vereda: verdict none
