// #10189: under --sv-comp a program that starts a thread is verified over its
// schedules, without the passes that assume one thread.
#include <assert.h>
#include <pthread.h>

int x;

void *set(void *arg)
{
  x = 1;
  return 0;
}

int main()
{
  pthread_t t;
  pthread_create(&t, 0, set, 0);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10189_thread
// Needs: --sv-comp --k-induction --interval-analysis
// RUN: --sv-comp --k-induction --interval-analysis
// EXPECT: ^The program can start a thread: verifying its schedules$
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
