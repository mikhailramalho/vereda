// A non-null pointer keeps its \result != 0 waypoint although no constant
// names its object: without it CPAchecker binds the null pointer's
// waypoint to the first call of the call site below, whose pointer is not
// null, and refutes the witness (#139, #76).
// RUN: --witness -
// EXPECT: value: "\\\\result != 0"(\n.*){3}\n +line: 13\n[\s\S]*value: "\\\\result == 0"(\n.*){3}\n +line: 13\n[\s\S]*value: "\\\\result == 4"
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_pointer_same_site.c", 3, "reach_error"); }
extern void *__VERIFIER_nondet_pointer(void);
extern int __VERIFIER_nondet_int(void);
void *get(void) {
  return __VERIFIER_nondet_pointer();
}
int main(void) {
  void *a = get();
  if (a == 0)
    return 0;
  void *b = get();
  if (b != 0)
    return 0;
  if (__VERIFIER_nondet_int() == 4)
    reach_error();
  return 0;
}
