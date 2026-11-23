// #10189: pthread_create in a function main never calls, as in Juliet's
// support code, leaves the program sequential, so k-induction proves it.
#include <assert.h>
#include <pthread.h>

void *work(void *arg)
{
  return 0;
}

void start_thread()
{
  pthread_t t;
  pthread_create(&t, 0, work, 0);
}

int main()
{
  unsigned i = 0;
  while (i < 10)
    ++i;
  assert(i == 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10189_no_thread_started
// Needs: --sv-comp --k-induction --interval-analysis --goto-unwind --unlimited-goto-unwind
// RUN: --sv-comp --k-induction --interval-analysis --goto-unwind --unlimited-goto-unwind
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*can start a thread)
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
