#include <pthread.h>
#include <string.h>
#include <assert.h>

void *readAndExecuteCommands(void *arg) {
  if (strcmp("a", "a"))
    assert(1);
}

int main(void) {
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, &readAndExecuteCommands, NULL);
  return 0;
}

// Imported from ESBMC regression/esbmc-unix/github_1351
// RUN: --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
