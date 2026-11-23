#include <stdlib.h>

void *main_thread(void *arg) {
  return NULL;
}

int main(int, int, int){  
  return 0;
}

// Imported from ESBMC regression/esbmc/github_270_1
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
