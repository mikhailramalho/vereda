/* The ASSERT half of the same path: a loop body writing an element its
 * __ESBMC_loop_assigns does not name must be reported, and reported by
 * element rather than by array. */
int global[8];

int main()
{
  int i = 0;

  __ESBMC_loop_assigns(i, global[3]);
  __ESBMC_loop_invariant(i >= 0 && i <= 4);
  while (i < 4)
  {
    global[3] = i;
    global[5] = i; /* not in __ESBMC_loop_assigns */
    i++;
  }

  return 0;
}
// Imported from ESBMC regression/loop-invariants/loop_assigns_array_elem_fail
// Needs: __ESBMC_loop_assigns, __ESBMC_loop_invariant
// Needs: --loop-invariant-check --loop-frame-rule
// RUN: --loop-invariant-check --loop-frame-rule --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*global\[5\] not in assigns clause.*$
