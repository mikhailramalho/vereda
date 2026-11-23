extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

struct s
{
  int f;
  int g;
};

int main(void)
{
  struct s t = {0};
  t.f = __VERIFIER_nondet_int();
  if (t.f == 7)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10770_initialised_struct_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)\A(?:(?!function_return).)*type: "function_return"(?:(?!function_return).)*\Z
// EXPECT: value: "\\\\result == 7"\n.*\n.*\n.*\n *line: 13\n
// Vereda: verdict none
