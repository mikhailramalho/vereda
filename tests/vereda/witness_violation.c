// --witness writes the counterexample as an SV-COMP violation witness: one
// function_return per nondet value, in the type's sign, and the target at
// the call to reach_error, not at the __assert_fail inside it. A waypoint
// gives the column of the call's closing parenthesis only when another call
// comes before it on the line; UAutomizer matches a nondet call in a
// declaration only at the declaration's ';', or by the line alone.
// RUN: --witness -
// EXPECT: entry_type: "violation_sequence"
// EXPECT: format_version: "2.0"
// EXPECT: specification: "CHECK\( init\(main\(\)\), LTL\(G ! call\(reach_error\(\)\)\) \)"
// EXPECT: data_model: "LP64"
// EXPECT: value: "\\\\result == 200"(\n.*){3}\n +line: 22\n +function: "main"\n
// EXPECT: value: "\\\\result == -5"(\n.*){3}\n +line: 23\n +column: 41\n
// EXPECT: type: "target"\n.*\n.*\n.*\n +line: 25\n +column: 5\n +function: "main"
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_violation.c", 3, "reach_error"); }
extern unsigned char __VERIFIER_nondet_uchar(void);
extern int __VERIFIER_nondet_int(void);
static int one(void) { return 1; }
int main(void) {
  unsigned char n = __VERIFIER_nondet_uchar();
  int x = one() + __VERIFIER_nondet_int();
  if (n == 200 && x == -4)
    reach_error();
  return 0;
}
