// Issue #9886: advance's loop sorts first, but every pass from its head leaves
// it (a CIL-style coroutine), so the cycle waypoint names main's loop.
extern int __VERIFIER_nondet_int(void);
int pc, n;
void advance(void)
{
  if (pc == 1)
    goto resume;
  while (n < __VERIFIER_nondet_int())
  {
    n = n + 1;
    pc = 1;
    goto out;
  resume:;
  }
out:;
}
int main(void)
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
  {
    if (x == 0)
      advance();
    x = 1;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/nonterm_witness_one_pass_loop_fail
// Needs: --termination --max-inductive-step 3 --witness-output-yaml -
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: non-terminating execution
// EXPECT: action: "cycle"
// EXPECT: line: 21$
// EXPECT: function: "main"
