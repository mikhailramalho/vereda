// Each target of p gets a step at the store, holding the same nondet in its
// struct literal; only the call's one value is named.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

struct pair
{
  int a, b;
};

struct pair g1, g2;

int main(void)
{
  struct pair *p = __VERIFIER_nondet_int() ? &g1 : &g2;
  p->b = __VERIFIER_nondet_int();
  if (p == &g1 && g1.b == 3)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10770_multi_target_first_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)\A(?:(?!function_return).)*(?:type: "function_return"(?:(?!function_return).)*){2}\Z
// EXPECT: value: "\\\\result == 3"\n.*\n.*\n.*\n *line: 16\n
// Vereda: verdict none
