/* Under --closed-world-fnptr a call through a pointer with no target cannot
 * happen, so the path past it is dead. */
extern int (*ext_cb)(int);

int main(void)
{
  int r = 0;
  r = ext_cb(1);
  __VERIFIER_assert(r == 0); // result unchanged
  return 0;
}
// Imported from ESBMC regression/esbmc/fnptr_no_target_result
// Needs: --closed-world-fnptr
// RUN: --ub-pointer-check=false --closed-world-fnptr
// MODE: FUTURE
// EXPECT: ^No target candidate for function call \*ext_cb; assuming unreachable \(--closed-world-fnptr\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
