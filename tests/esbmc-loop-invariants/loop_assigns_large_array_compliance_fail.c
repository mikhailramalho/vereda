/* The body also writes global[5], outside the clause. */
int global[512];

int main()
{
  int i = 0;

  __ESBMC_loop_assigns(i, global[3]);
  __ESBMC_loop_invariant(i >= 0 && i <= 4);
  while (i < 4)
  {
    global[3] = 1;
    global[5] = 2;
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/loop-invariants/loop_assigns_large_array_compliance_fail
// Needs: --loop-invariant-check --loop-frame-rule
// RUN: --loop-invariant-check --loop-frame-rule
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+10\s+assigns compliance: c:@global not in assigns clause$
// Vereda: verdict none
