#include <pthread.h>
#include <assert.h>

void __ESBMC_yield();

int s = 0;
void *my_thread(void *arg) {
   s++; 
   assert(s == 1); 
   s--;
}

#include <pthread.h>
int main(){
   pthread_t id;
   while(1) {
     pthread_create(&id, NULL, my_thread, NULL);
   }
}

// Imported from ESBMC regression/esbmc-unix2/01_pthread46
// RUN: --unwind 2 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
