// Testing allocation with a nondet number

int bar(void *ptr) {
  unsigned int size = __ESBMC_get_object_size(ptr); // Do we have something like this?
  __VERIFIER_assert(size < 70); // Allocated size should be equal
}

int main() {
    unsigned int alloc_size = __VERIFIER_nondet_uint();
    __VERIFIER_assume(alloc_size > 0 && alloc_size < 60);
    void * foo = malloc(alloc_size);
    bar(foo); 
}
// Imported from ESBMC regression/esbmc/github_636_1_true
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
