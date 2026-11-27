// get_formated_assignment() drops an assumption whose value is an aggregate
// literal, so a write through a pointer used to contribute nothing to the
// violation witness. Reporting the component puts it back.
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
// Imported from ESBMC regression/esbmc/counterexample_witness_assumption_fail
// Needs: --witness-output-graphml
// RUN: --witness-output-graphml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^      <data key="assumption">p\.in == 7;</data>$
// EXPECT: ^      <data key="assumption">q-&gt;out == 21;</data>$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
