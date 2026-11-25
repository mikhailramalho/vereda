// As counterexample_component_value_fail, for an lvalue reached through a
// pointer: `q->out` names a component of the object the dereference resolved
// to, so the step reports that component, not the whole object.
struct pair
{
  int in;
  int out;
};

struct pair p;

static void scale(struct pair *q)
{
  q->out = q->in * 3;
}

int main(void)
{
  p.in = 7;
  scale(&p);
  __VERIFIER_assert(p.out != 21); // out is 21
  return 0;
}
// Imported from ESBMC regression/esbmc/counterexample_deref_component_value_fail
// EXPECT: ^  p\.in = 7;\n  bits: 00000000 00000000 00000000 00000111$
// EXPECT: ^  q->out = 21;\n  bits: 00000000 00000000 00000000 00010101$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  q->out = 21 \(00000000 00000000 00000000 00010101\)$
// MODE: FUTURE
// Vereda: verdict FAILED
