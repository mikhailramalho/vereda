/* #10296: --schedule --state-hashing crashed in generate_hash. */
#include <pthread.h>
#include <assert.h>
int x = 0;
void *t(void *a) { x = 5; return 0; }
int main() { pthread_t a; pthread_create(&a, 0, t, 0); assert(x == 5); return 0; }
// Imported from ESBMC regression/esbmc-unix/github_10296_state_hashing_reject
// Needs: --schedule
// RUN: --schedule
// MODE: FUTURE
// EXPECT: ^ERROR: --schedule cannot be combined with --state-hashing$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
