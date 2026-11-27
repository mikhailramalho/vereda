// An aggregate element is reported element-wise, and an element the model
// answers with a cast rather than a value keeps the whole element from being
// reported as one: the walk over the value's operands decides that.
union u
{
  int i;
  char c[4];
};

struct holder
{
  union u un;
  int t;
};

struct holder harr[2];

int main(void)
{
  struct holder v;
  v.un.i = 9;
  v.t = 4;
  harr[0] = v;
  __VERIFIER_assert(harr[0].t != 4); // t is 4
  return 0;
}
// Imported from ESBMC regression/esbmc/counterexample_aggregate_component_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  harr\[0\] = \{ \.un=9, \.t=4 \}$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  harr\[0\] = \{ \.un=9, \.t=4 \}$
// MODE: FUTURE
// Vereda: verdict FAILED
