// --witness writes the counterexample as an SV-COMP violation witness: one
// function_return per nondet value, at the call's closing parenthesis, in
// the type's sign, and the target at the call to reach_error, not at the
// __assert_fail inside it.
// RUN: --witness -
// EXPECT: entry_type: "violation_sequence"
// EXPECT: format_version: "2.0"
// EXPECT: specification: "CHECK\( init\(main\(\)\), LTL\(G ! call\(reach_error\(\)\)\) \)"
// EXPECT: data_model: "LP64"
// EXPECT: value: "\\\\result == 200"(\n.*){3}\n +line: 19\n +column: 45\n
// EXPECT: value: "\\\\result == -5"(\n.*){3}\n +line: 20\n +column: 33\n
// EXPECT: type: "target"\n.*\n.*\n.*\n +line: 22\n +column: 5\n +function: "main"
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_violation.c", 3, "reach_error"); }
extern unsigned char __VERIFIER_nondet_uchar(void);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  unsigned char n = __VERIFIER_nondet_uchar();
  int x = __VERIFIER_nondet_int();
  if (n == 200 && x == -5)
    reach_error();
  return 0;
}
