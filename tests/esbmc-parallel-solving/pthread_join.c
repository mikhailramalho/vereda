#include <pthread.h>

int main() 
{ 
    for (;;) 
        pthread_join(0, 0);
}
// Imported from ESBMC regression/parallel-solving/pthread_join
// Needs: --parallel-solving --k-induction
// RUN: --parallel-solving --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
