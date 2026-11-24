// A nondet function declared without a prototype, as in many SV-COMP
// tasks, still gets its function_return waypoints. On i386 clang declares
// it variadic but calls it without the "...", with the same result type.
// RUN: -m32 --witness -
// EXPECT: value: "\\\\result == 4000000000"(\n.*){3}\n +line: 13\n +column: 43\n
// EXPECT: value: "\\\\result == -3"(\n.*){3}\n +line: 14\n +column: 35\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error() { __assert_fail("0", "witness_unprototyped.c", 3, "reach_error"); }
extern unsigned int __VERIFIER_nondet_uint();
extern char __VERIFIER_nondet_char();
int main() {
  unsigned int n = __VERIFIER_nondet_uint();
  char c = __VERIFIER_nondet_char();
  if (n == 4000000000U && c == -3)
    reach_error();
  return 0;
}
