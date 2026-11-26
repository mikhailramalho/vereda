/* A field written through its own variable under --scalarize-structs (#7521)
 * shows in the counterexample under the field's name. */
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
// Imported from ESBMC regression/esbmc/scalarize_structs_fail
// RUN: --unwind 5 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Scalarized 1 struct variable\(s\)$
// EXPECT: ^  s\.total = 18; // 00000000 00000000 00000000 00010010$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict FAILED
