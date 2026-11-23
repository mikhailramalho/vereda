// A nondet call a macro expands to is located at the macro's name, where
// the closing parenthesis cannot be found; the waypoint leaves the column
// out instead of pointing at the ')' of another call on the line.
// RUN: --witness -
// EXPECT: value: "\\\\result == 5"(\n.*){3}\n +line: 14\n +function: "main"
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_macro_call.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
#define NONDET __VERIFIER_nondet_int()
static int g(int x) { return x; }
static int f(int x) { return x + 3; }
int main(void) {
  if (NONDET == f(g(2)))
    reach_error();
  return 0;
}
