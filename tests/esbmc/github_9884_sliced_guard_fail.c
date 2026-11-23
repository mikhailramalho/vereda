// #9884: the inner branch reads only x, which the overflow check keeps, but
// its guard reads the sliced definition of the outer branch's guard. It must
// get no waypoint; the outer branch keeps its correct one.
extern int __VERIFIER_nondet_int(void);
extern void
__assert_fail(const char *, const char *, unsigned int, const char *);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = 0;
  if (x > 5)
    y = 1;
  else
  {
    if (x < 0)
      __assert_fail("0", "main.c", 17, "main");
  }
  return x + 1;
}
// Imported from ESBMC regression/esbmc/github_9884_sliced_guard_fail
// Needs: --no-assertions --witness-output-yaml -
// RUN: --overflow-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: value: "true"\n\s+location:\n\s+file_name: ".*github_9884_sliced_guard_fail\.c"\n\s+line: 12$
// EXPECT: type: "target"
// EXPECT: (?s)\A(?!.*line: 16$)
