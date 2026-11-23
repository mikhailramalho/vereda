#ifndef __has_builtin        // Optional of course.
#  define __has_builtin(x) 0 // Compatibility with non-clang compilers.
#endif

int main()
{
#if __has_builtin(__builtin_malloc) && __has_builtin(__builtin_free)
  char *p = (char *)__builtin_malloc(5);
  __builtin_free(p);
  __builtin_free(p); // double free
  return 0;
#else
  __VERIFIER_assert(
    0); // This test requires __builtin_malloc and __builtin_free support
#endif
}
// Imported from ESBMC regression/esbmc/builtin_free-fail
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION FAILED
// Vereda: unmet EXPECT: ^VERIFICATION FAILED
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
