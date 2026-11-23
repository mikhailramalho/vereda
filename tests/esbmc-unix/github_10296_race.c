#include <pthread.h>
int x = 0;
void *t(void *a) { x = 1; return 0; }
int main() { pthread_t a; pthread_create(&a, 0, t, 0); x = 2; pthread_join(a,0); return 0; }
// Imported from ESBMC regression/esbmc-unix/github_10296_race
// Needs: --schedule --data-races-check
// RUN: --schedule --data-races-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
