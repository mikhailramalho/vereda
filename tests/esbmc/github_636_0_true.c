// Testing allocation with a fixed number

unsigned int MAGIC_NUMBER = 32;

int bar(void *ptr) {
  unsigned int size = __ESBMC_get_object_size(ptr); // Do we have something like this?
  __VERIFIER_assert(size == (MAGIC_NUMBER)); // Allocated size should be equal
}

int main() {
    void * foo = malloc(MAGIC_NUMBER);
    bar(foo); 
}
// Imported from ESBMC regression/esbmc/github_636_0_true
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
