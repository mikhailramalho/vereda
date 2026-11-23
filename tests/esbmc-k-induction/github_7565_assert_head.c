/* An ASSERT loop head makes havoc_slot step back onto the entry `goto`, so that
 * one jump is both the legacy havoc slot and an entry jump. It must be havoced
 * once, not twice (#7565). */

_Bool a = 1;
_Bool b = 0;

int main()
{
  goto c;
d:
  __VERIFIER_assert(b);
c:
  b = a;
  a = 0;
  goto d;
}
// Imported from ESBMC regression/k-induction/github_7565_assert_head
// Needs: --k-induction --no-remove-unreachable --goto-functions-only
// RUN: --k-induction --no-remove-unreachable --goto-functions-only
// MODE: FUTURE
// EXPECT: main \(c:@F@main\):\n[^\n]*\n\s*ASSIGN a=NONDET\(_Bool\);\n[^\n]*\n\s*ASSIGN b=NONDET\(_Bool\);\n[^\n]*\n\s*GOTO \d+\n
// Vereda: verdict none
