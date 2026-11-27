// A component the model does not pin down -- get() reads a pointer-typed
// member back as an expression, not a value -- keeps reporting the object the
// SSA assignment rewrote, rather than a step with no value at all.
struct holder
{
  int *p;
  int t;
};

struct holder h;
int x = 5;

int main(void)
{
  h.p = &x;
  h.t = 3;
  __VERIFIER_assert(h.t != 3); // t is 3
  return 0;
}
// Imported from ESBMC regression/esbmc/counterexample_unresolved_component_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  h\.p = \{ \.p=&x, \.t=0 \}$
// EXPECT: ^  h\.t = 3; // 00000000 00000000 00000000 00000011$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  h\.p = \{ \.p=&x, \.t=0 \}$
// MODE: FUTURE
// Vereda: verdict FAILED
