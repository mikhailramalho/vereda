// #9884: with --no-assertions the slicer removes the definition of cond, so
// the branch in check() must get no waypoint rather than an arbitrary one.
extern int __VERIFIER_nondet_int(void);
extern void
__assert_fail(const char *, const char *, unsigned int, const char *);

void check(int cond)
{
  if (!(cond))
    __assert_fail("0", "main.c", 10, "check");
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  check(x > 5);
  return x + 1;
}
// Imported from ESBMC regression/esbmc/github_9884_sliced_cond_fail
// Needs: --no-assertions --witness-output-yaml -
// RUN: --overflow-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"
// EXPECT: (?s)\A(?!.*type: "branching")
