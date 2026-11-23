// #10749: only the schedule is nondeterministic; the witness orders the threads.
#include <pthread.h>
extern void abort(void);
void reach_error(void) { abort(); }
void __VERIFIER_atomic_begin(void);
void __VERIFIER_atomic_end(void);
int i = 3, j = 6;
void *t1(void *arg) {
  for (int k = 0; k < 3; k++) { __VERIFIER_atomic_begin(); i = j + 1; __VERIFIER_atomic_end(); }
  pthread_exit(0);
}
void *t2(void *arg) {
  for (int k = 0; k < 3; k++) { __VERIFIER_atomic_begin(); j = i + 1; __VERIFIER_atomic_end(); }
  pthread_exit(0);
}
int main(void) {
  pthread_t id1, id2;
  pthread_create(&id1, 0, t1, 0);
  pthread_create(&id2, 0, t2, 0);
  __VERIFIER_atomic_begin(); int condI = i >= (2*3 + 6); __VERIFIER_atomic_end();
  __VERIFIER_atomic_begin(); int condJ = j >= (2*3 + 6); __VERIFIER_atomic_end();
  if (condI || condJ) { ERROR: {reach_error(); abort();} }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10749_schedule_witness_fail
// Needs: --error-label ERROR --witness-output-yaml -
// RUN: --error-label ERROR --unwind 4 --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^    format_version: "2\.4"$
// EXPECT: (?s)type: "function_enter"(?:(?!waypoint:).)*line: 18\s+column: 32
// EXPECT: (?s)type: "function_enter"(?:(?!waypoint:).)*line: 19\s+column: 32
// EXPECT: (?s)function: "t1"\s+thread_id: 1.*function: "t2"\s+thread_id: 2.*function: "t1"\s+thread_id: 1
// EXPECT: (?s)type: "target"(?:(?!thread_id).)*\Z
// Vereda: verdict none
