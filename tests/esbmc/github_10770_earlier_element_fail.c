// read_sensor() has no body, so its result is nondet, but it is not a
// __VERIFIER_nondet_*() call and gets no waypoint. Its value is still in the
// array the later call's store is folded into, which must not stop that call
// from getting its own waypoint.
extern int __VERIFIER_nondet_int(void);
extern int read_sensor(void);
extern void reach_error(void);

int main(void)
{
  int a[2] = {0};
  a[0] = read_sensor();
  a[1] = __VERIFIER_nondet_int();
  if (a[0] == 4 && a[1] == 9)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10770_earlier_element_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)\A(?:(?!function_return).)*type: "function_return"(?:(?!function_return).)*\Z
// EXPECT: value: "\\\\result == 9"\n.*\n.*\n.*\n *line: 13\n
// Vereda: verdict none
