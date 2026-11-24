// A null pointer gets \result == 0. No constant names the object another
// pointer points into, so it gets \result != 0, which still puts a waypoint
// at its call.
// RUN: --witness -
// EXPECT: value: "\\\\result == 0"(\n.*){3}\n +line: 15\n
// EXPECT: value: "\\\\result != 0"(\n.*){3}\n +line: 16\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_pointer.c", 3, "reach_error"); }
extern void *__VERIFIER_nondet_pointer(void);
extern char *__VERIFIER_nondet_pchar(void);
int main(void) {
  // Each call has a line of its own so that its waypoint shows by its
  // line.
  void *p = __VERIFIER_nondet_pointer();
  char *q = __VERIFIER_nondet_pchar();
  if (p == 0 && q != 0)
    reach_error();
  return 0;
}
