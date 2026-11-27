#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char buffer[100];
    char *ptr = buffer + argc; // symbolic offset
    size_t size = __builtin_object_size(ptr, 1);
    if(argc > 100)
      assert(size == (size_t)0);
    else
      Assert((int) size == (100 - argc));      
    return 0;
}

// Imported from ESBMC regression/extensions/builtin_object_size11
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
