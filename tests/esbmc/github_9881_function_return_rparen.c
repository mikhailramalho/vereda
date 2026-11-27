/* #9881: a function_return waypoint sits at the call's ')' and names the
 * value the call returned (3), not the value of x (7). A call assigned
 * directly (y) gets one too. */
extern int __VERIFIER_nondet_int(void);
extern unsigned char __VERIFIER_nondet_uchar(void);

int main(void)
{
  int x = 1 + 2 * __VERIFIER_nondet_uchar();
  int y;
  y = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 7 || y != 5); // x is 7 and y is 5
}
// Imported from ESBMC regression/esbmc/github_9881_function_return_rparen
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: value: "\\\\result == 3"\n.*\n.*\n.*\n *line: 9\n *column: 43$
// EXPECT: value: "\\\\result == 5"\n.*\n.*\n.*\n *line: 11\n *column: 29$
// Vereda: verdict none
