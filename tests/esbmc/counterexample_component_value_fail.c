// A counterexample step prints an lvalue and a value; the value must be the
// one that lvalue holds. symex rewrites `s.total = ...` into a whole-object
// update of `s`, so reading the step's RHS answers for all of `s` instead.
struct sample
{
  int total;
  int reading[4];
};

struct sample s;

int main(void)
{
  for (int i = 0; i < 4; i++)
  {
    s.reading[i] = i * 3;
    s.total = s.total + s.reading[i];
  }

  __VERIFIER_assert(s.total != 18); // total is 18
  return 0;
}
// Imported from ESBMC regression/esbmc/counterexample_component_value_fail
// RUN: --unwind 5 --no-unwinding-assertions
// EXPECT: ^  s\.reading\[1\] = 3;\n  bits: 00000000 00000000 00000000 00000011$
// EXPECT: ^  s\.total = 3;\n  bits: 00000000 00000000 00000000 00000011$
// EXPECT: ^  s\.reading\[3\] = 9;\n  bits: 00000000 00000000 00000000 00001001$
// EXPECT: ^  s\.total = 18;\n  bits: 00000000 00000000 00000000 00010010$
// EXPECT: ^VERIFICATION FAILED$
