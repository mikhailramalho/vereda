/* A call through a pointer with no target is skipped in the open world, so its
 * result is unknown, not the value the left-hand side held before. */
extern int (*ext_cb)(int);

int main(void)
{
  int r = 0;
  r = ext_cb(1);
  __VERIFIER_assert(r == 0); // result unchanged
  return 0;
}
// Imported from ESBMC regression/esbmc/fnptr_no_target_result_fail
// RUN: --no-pointer-check
// EXPECT: ^No target candidate for function call \*ext_cb$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^No target candidate for function call \*ext_cb$
// MODE: FUTURE
// Vereda: verdict FAILED
