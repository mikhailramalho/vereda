#include <pthread.h>

int a;

void* t1(void *arg) {
    a = 1;                 
    return NULL;
}

int t2(void) {
    return a;
}

int main() {
    pthread_t id1, id2;

    pthread_create(&id1, NULL, t1, NULL);
    t2();

    return 0;
}
// Imported from ESBMC regression/esbmc-unix/SV_COMP_02
// Needs: --data-races-check
// RUN: --data-races-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
