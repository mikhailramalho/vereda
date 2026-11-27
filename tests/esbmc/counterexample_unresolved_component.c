// The holding counterpart of counterexample_unresolved_component_fail.
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
  __VERIFIER_assert(h.t == 3); // t is not 3
  return 0;
}
// Imported from ESBMC regression/esbmc/counterexample_unresolved_component
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
