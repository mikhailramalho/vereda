#include <stdlib.h>

void *main_thread(void *arg) {
  return NULL;
}

int main(void){  
  return 0;
}

// Imported from ESBMC regression/esbmc/github_270
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
